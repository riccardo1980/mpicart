# mpicart
Facilities for halo exchange on MPI cartesian grid topology.

## Getting Started
Use Makefile to compile test examples.

### Prerequisites
A working MPI environment: mpic++ is required to compile sources.

### Installing
Simply download the zip file, or clone the project using:

``` 
git clone https://github.com/riccardo1980/mpicart.git mpicart
```
then use `cmake` on an empty build folder:

```
cd mpicart
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
For debug version, use `-DCMAKE_BUILD_TYPE=Debug` flag on cmake step.

### Tested Architectures

| OS                       | compiler                  | MPI library |
| ------------------------ | ------------------------- | ----------- |
| Linux x86_64 (Debian)    | g++ 6.2.0                 | MPICH 3.2   |
| OS X 10.11.6 (El Capitan)| g++ 6.2.0 (MacPorts gcc6) | MPICH 3.2   |

## Author

* **Riccardo Zanella** [riccardo1980](https://github.com/riccardo1980)

## License
This project is licensed under GPL v.2 - see the [LICENSE.txt](LICENSE.txt) file for details. 

