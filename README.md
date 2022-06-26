# Criptografía y Seguridad
## Esteganografía - Trabajo Práctico de Implementación
### ITBA 2022 1C
 ---------------------------------------------------------

## Autores

- [Serpe, Octavio](github.com/oserpe)
- [Quesada, Francisco](github.com/fquesada00)
- [Arca, Gonzalo](github.com/gonzaloarca)

## Dependencias

- Compilador de C (Probado en GCC 9.4.0 y Apple Clang 13.1.6)
- OpenSSL (Probado en versión 1.1.1n)
- Librería de OpenSSL (libssl-dev para Linux)

## Cómo compilar el proyecto
Desde la terminal situada en la carpeta raíz del proyecto ejecutar
```bash
$ make all
```

Para habilitar debugging y compilar con símbolos, ejecutar
```bash
$ make debug
```

## Cómo ejecutar el proyecto
Referirse al enunciado del trabajo práctico
## Cómo ejecutar pruebas
Para correr el programa con todos los algoritmos de esteganografiado (ocultar y extraer información), ejecutar el siguiente comando:
```bash
$ python3 runner.py -c <archivo_portador> -m <archivo_a_ocultar> -o <archivo_esteganografiado> -out <nombre_archivo_extraido> -p <contraseña>
```
donde:

- `<archivo_portador>` es el archivo donde se ocultará el mensaje. El formato debe ser BMP en su versión 3. Requerido.
- `<archivo_a_ocultar>` es el archivo que se ocultará en el portador. Requerido.
- `<archivo_esteganografiado>` es el archivo resultante de ocultar la información. El formato debe ser BMP. Requerido.
- `<nombre_archivo_extraido>` es el nombre del archivo que se extraerá del portador. Requerido.
- `<contraseña>` es la contraseña que se usará para desencriptar el archivo. Opcional.

> En el caso que se especifique una contraseña, se ejecutará el programa con cada uno de los modos de cifrado por bloque disponible con cada uno de los modos de operación disponibles bajo los algoritmos de esteganografiado disponibles.

