# Solo5 : SeL4 implementation

## Setup and build

1. To initialize and setup the SeL4 build system, just run the `update.sh` script, to install the other components (SeL4 kernel + libs and toolchain). 
Note that this script can be run again to git-pull all the repos.
2. Create a build folder, e.g `build`
3. Run the initialization script inside the build folder:

```bash
../init-build.sh  -DPLATFORM=x86_64 -DSIMULATION=TRUE
```
4. And start the build process

```bash
ninja
```

If everything goes well, you can run the generated image in QEMU with `./simulate`

More informations about SeL4's build system can be found here : <https://docs.sel4.systems/Developing/Building/Incorporating>

## Implementation notes
In progress :)
