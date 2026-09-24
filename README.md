# forth

## build
```sh 
rm -rf build
cmake -S . -B build
cmake --build build
```

## run tests
```sh ctest --test-dir build --output-on-failure``` 