# Filtro de Imagenes en Escala de Grises

## Prerequisitos

- Compilador C/C++: 
```
sudo apt-get install build-essential  
```
- Libreria OpenMP
```
sudo apt-get install libomp-dev 
```

## Compilacion de codigo .c
`gcc -o [archivo de salida] -fopenmp [archivo .c]`

```
gcc -o imagen -fopenmp imagen.c    
```

## Ejecucion de ejecutable .c

`./[archivo de salida]`

```
./imagen
```

## Transformacion de JPG a PPM
`convert [imagen jpg] -compress none [archivo de salida]`
```
convert auto_azul.jpg -compress none auto_azul_p3.ppm 
```