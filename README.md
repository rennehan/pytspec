# pytspec
Calculating spectroscopic temperature (Vikhlinin 2006).

## Installation
Simply run "make" in the top-level directory after cloning, and it will install the pytspec package. If you encounter an error about linking, please edit the lib/Makefile to have "gcc -fPIC" rather than "gcc" as the C-compiler. Make sure that you "make clean" in the /lib directory before you try to "make" in the top-level directory again to avoid the error.

## Usage

Once installed, you can import pytspec into any Python code. The code requires calibration files following Vikhlinin (2006), who provides the "mk_cal" software. See their paper for more info.

Here is an example script with fake data. The list could be simulation data of all of the gas within a cluster, for example. These must be converted to Python arrays in order to pass to the C code. *emission_measure* is \int n_H n_e dV.

```python
import pytspec
import array

calibration = b'test.dat'                                       # b so we can pass to C
temperatures = array.array('f', [0.3, 0.3, 0.3])                # in keV
abundances = array.array('f', [0.1, 0.1, 0.1])                  # in Solar 
emission_measures = array.array('f', [0.2, 0.2, 0.2])      # \int nh * ne * dV

tspec = pytspec.calculate(calibration, temperatures, abundances, emission_measures)
```
