
# soren

This is a work in progress FE9/FE10 event script/CMB file processor. Right now all it can do is kind of dump scripts in a semi-readable format.

## usage

    soren <path/to/script.cmb>

Will print dump to stdout.

Example output in its current state (this is the last event in the `Scripts/C02.cmb` from the US version of FE9):

    EVENT unk_28()
    {
      push [&var_0] = UnitGetByPID("PID_IKE");
      UnitAddItem(var_0, "IID_IRONSWORD");
      UnitAddItem(var_0, "IID_IRONSWORD");
      UnitAddItem(var_0, "IID_IRONSWORD");
      return 0;
    }

## build

Have CMake and a C++ compiler.

    mkdir build
    cd build
    cmake ..
    cmake --build .

Eventually (when the compiler will be implemented), this will also require RE2C and maybe lemon.
