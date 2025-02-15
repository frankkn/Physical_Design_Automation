# Placement Legalization
Implement an existing algorithm, called “Abacus”, to legalize a given global placement result with minimal displacement.

## How to Compile
In this directory, enter the following command:
```
$ make
```
It will generate the executable file "hw4" in "Placement_Legalization/bin/".

If you want to remove it, please enter the following command:
```
$ make clean
```

## How to Run
**In this directory, enter the following command:**
```
Usage: ../bin/<exe> <aux file>
```

e.g.
```
$ ../bin/hw4 ../testcase/adaptec1/adaptec1.aux
```

**In "Placement_Legalization/bin/", enter the following command:**
```
Usage: ./<exe> <aux file>
```

e.g.
```
$ ./hw4 ../testcase/adaptec1/adaptec1.aux
```

## How to Test
In this directory, enter the following command:
```
$ ../verifier/verify $(testcase path) $(result path)
```
It will test on $(testcase) and run verifier to verify the answer.

e.g. test on adaptec1 and verify the answer
```
$ ../verifier/verify ../testcase/adaptec1/adaptec1.aux ../output/adaptec1.result
```

## Compile, Run and Test via run.sh
In this directory, enter the following command:
```
$ ./run.sh
$ y
$ ibm01
```
It will compile, run and test(verify) the answer.
