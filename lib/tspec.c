#include "tspec.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int gtindex;
int NTEMP;
float tcal[100], emean_cal[100];

/**
 * Converted code from Vikhlinin 2006 to calculate spectroscopic temperature.
 * - D. Rennehan 2019
 */
float calculate_tspec(char cal_file_name[], float *t, float *a, float *n2, char mode[], int nt)
{
    FILE * calibration_file;

    if ((calibration_file = fopen(cal_file_name, "r")) == NULL)
    {
        printf("Cannot open calibration file %s\n", cal_file_name);
        exit(-1);
    }

    float fcont_cal[100], fline_cal[100];
    char line[256];
    int idx = 0, ncal = 0;
    while (fgets(line, 256, calibration_file) != NULL)
    {
        sscanf(line, "%g %g %g %g", &tcal[idx], &fcont_cal[idx], &fline_cal[idx], &emean_cal[idx]);
        idx++;
    }

    fclose(calibration_file);

    ncal = idx;
    printf("Read in %d lines from calibration file.\n", ncal);
    NTEMP = ncal;

    //  real t(nt)    --- array of input temperatures
    //  real a(nt)    --- array of input metallicities
    //  real n2(nt)   --- array of input emission measures
    //  real tcal(nt), fcont_cal(nt), fline_cal(nt), emean_cal(nt) -- calibration arrays
    //  Tmean, Tcont, Tline --- "spectro-T", "continuum-T", "line-T" (see PAPER)

    int i;
    float Tline, Tcont, wcont, wline;
    float acont, beta, delta1, delta2, fcont, fline, x;
    float fluxline, fluxcont, emean;
    float Tmean_ret;

    if (mode == "xmm/pn" || mode == "XMM/PN")
    { 
        acont = 0.79;
        beta = 0.75;
        delta1 = 0.270;
        delta2 = 0.225;
    } else if (mode == "xmm/mos" || mode == "xmm/mos") {
        acont = 0.90;
        beta = 1;
        delta1 = 0.19;
        delta2 = 0.22;
    } else if (mode == "xmm/mos+pn" || mode == "XMM/MOS+PN") {
        acont = 0.91;
        beta = 0.90;
        delta1 = 0.19;
        delta2 = 0.21;
    } else if (mode == "asca/sis" || mode == "ASCA/SIS") {
        acont = 0.875;
        beta = 0.80;
        delta1 = 0.20;
        delta2 = 0.22;
    } else if (mode == "asca/gis" || mode == "ASCA/GIS") {
        acont = 0.79;
        beta = 0.75;
        delta1 = 0.26;
        delta2 = 0.30;
    } else { /* Chandra */
        acont = 0.875;
        beta = 1;
        delta1 = 0.19;
        delta2 = 0.25;
    }

    wline = 0.0;
    wcont = 0.0;
    Tline = 0.0;
    Tcont = 0.0;
    fluxline = 0.0;
    fluxcont = 0.0;

    for (i = 0; i < nt; i++)
    {
        //     Find continuum and line flux, and average energy of the line emission
        find_temp_index(t[i]);
	fcont = fcont_cal[gtindex] + (t[i] - tcal[gtindex]) / (tcal[gtindex + 1] - tcal[gtindex]) * (fcont_cal[gtindex + 1] - fcont_cal[gtindex]);
	fline = fline_cal[gtindex] + (t[i] - tcal[gtindex]) / (tcal[gtindex + 1] - tcal[gtindex]) * (fline_cal[gtindex + 1] - fline_cal[gtindex]);
	emean = emean_cal[gtindex] + (t[i] - tcal[gtindex]) / (tcal[gtindex + 1] - tcal[gtindex]) * (emean_cal[gtindex + 1] - emean_cal[gtindex]);

        //     Multiply fluxes by emission measure and metallicity
        fcont = fcont * n2[i];
        fline = fline * n2[i] * a[i];
        
        //     eq.[6] in the paper
        Tcont = Tcont + t[i] * fcont / pow(t[i], acont);
        wcont = wcont + fcont / pow(t[i], acont);
        fluxcont = fluxcont + fcont;
        
        //     eq.[2] in the paper
      	Tline = Tline + emean * fline;
        wline = wline + fline;
    }

    //     eq.[4] in the paper
    Tcont = Tcont / wcont;

    //     eq.[3] in the paper
    emean = Tline / wline;
    find_energy_index(emean);
    Tline = tcal[gtindex] + (emean - emean_cal[gtindex]) / (emean_cal[gtindex + 1] - emean_cal[gtindex]) * (tcal[gtindex + 1] - tcal[gtindex]);
    fluxline = wline;
        
    //     eq.[7,8,12] in the paper
    x = fluxcont / (fluxcont + fluxline);
    wcont = exp(-pow(pow(x - 1, 2) / pow(delta1, 2), beta)) * exp(-pow(pow(x - 1, 2) / pow(delta2, 2), 4.0));
    Tmean_ret = wcont * Tcont + (1 - wcont) * Tline;

    return Tmean_ret;
}

void find_temp_index(float temperature)
{
    int i;
    // Deal with the case where the temperature is lower than the minimum temperature
    // Set it to the minimum temperature
    if (temperature < tcal[0])
    {
        gtindex = 0;
    }
    // Deal with the case where the temperature is higher than the maximum temperature
    // Set it to the maximum temperature
    else if (temperature > tcal[NTEMP - 1]) {
        gtindex = NTEMP - 2;
    } else {
        for (i = 1; i < NTEMP; i++)
        {
	    // The temperature values are ascendingly ordered. Break out of the loop
	    // when we exceed the particle temperature
	    if (tcal[i] > temperature)
	    {
	        gtindex = i - 1;
		break;
	    }	
	}
    }
	
    return;
}

void find_energy_index(float energy)
{
    int i;
	
    // Deal with the case where the temperature is lower than the minimum temperature
    // Set it to the minimum temperature
    if (energy < emean_cal[0])
    {
        gtindex = 0;
    }
    // Deal with the case where the temperature is higher than the maximum temperature
    // Set it to the maximum temperature
    else if (energy > emean_cal[NTEMP - 1]) {
        gtindex = NTEMP - 2;
    } else {
        for (i = 1; i < NTEMP ; i++)
        {
            // The temperature values are ascendingly ordered. Break out of the loop
            // when we exceed the particle temperature
            if (emean_cal[i] > energy)
            {
                gtindex = i - 1;
		break;
	    }		
	}
    }
	
    return;
}

