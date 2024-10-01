# Models for DDFastShowerML

The models in this directory are (partially) dynamically downloaded depending on
the value of the `DOWNLOAD_MODELS` CMake option. If it is `ON` the models will
be fetched as part of the build process and then copied back to this directory.
The model binaries are stored on the Key4hep EOS space from where they can be
fetched via https using

```bash
wget https://key4hep.web.cern.ch:443/MLFastShowerModels/<model-name>
```

When building the library all the fetching is handled by CMake using the
`ExternalData` module.

## Adding a new model
Adding a new model consists of
- Uploading the model binary file to EOS and creating a symlink with the
  `md5sum` of the file as name (`ln -s <file> $(md5sum <file> | awk '{print
  $1}'`). This is important, because `ExternalData` expects an object store, and
  we are leveraging the fact that EOS's https access redirects to the actual
  file when there is a symlink.
- Creating a file `<file>.md5` in the `models` directory where the content is
  the `md5sum` from step 1 (i.e. the symlink name)
- Adding `<file>` to the `models` list at the top of `models/CMakeLists.txt`
