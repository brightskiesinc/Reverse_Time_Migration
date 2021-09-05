# TODO

### Todo

- Add option to build it either as static or shared library.
- Configurable interface: make acquire function take the configuration map, instead of implicitly making each class that
  inherits do a special constructor. Due to it "ConfigurationMap" being an interface, it has to be a pointer in Acquire
  properties, but that will cause ownership problems, so either a WrapperClass approach, or shared pointer(A wrapper
  class might be better here, refer to SeismicReader in Thoth library).
- ConfigurationMap interface needs to consolidate all configuration maps everywhere
- Allow Exceptions to get an additional string to be able to add common info when throwing them.
- Create tests for:
    * `JSONConfigurationMap`
- Add `Logging` module (Shall be taken from `SeismicToolbox/libs/SeismicOperations`)
- Add `Backend` module (Shall be taken from `SeismicToolbox/libs/SeismicOperations`)
- Add `Timer` module (When `Timer` library is completely finished, it shall then be moved to base package)

### In Progress

- [ ] PLACE-HOLDER

### Done

- [x] PLACE-HOLDER
