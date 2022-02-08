txtMap
======

Una herramienta para crear aventuras conversacionales, a partir de un texto creado como si se jugara.

<p>
<a href="http://www.caad.es/baltasarq/prys/txtmap/">Web</a>

Uso
---

**txtMAP** es una herramienta de línea de comando. Toma un archivo fuente y lo convierte en archivo de destino.

`$ txtmap --PL=XXX <archivo>`

Donde *archivo* es el nombre de un archivo que contiene una descripción de la aventura a medida que se juega. *XXX* es el nombre del *plugin* elegido.

El archivo *leeme.txt* se incluye a modo de documentación para inicio rápido.

Lo que le dice qué hacer con el archivo fuente es el *plugin* utilizado. Un listado total de plugins puede obtenerse con la siguiente orden.

```
$ txtmap --PL? leeme.txt
Los plugins aceptados son:
AGE                      - Genera archivo compilable por AGE.
BAS                      - Genera un archivo para BASIC.
FI.JS                    - Genera un archivo para fi.js.
GLULXI6GB                - Genera un archivo para Glulx con Inform 6 (librería inglesa).
GLULXI6I18N              - Genera un archivo para Glulx con Inform 6 (internacional).
GLULXINFORMATE!          - Genera un archivo para Glulx con Inform 6 (librería española).
HTML                     - Genera un conjunto de páginas web, una por localidad.
I6I18N                   - Genera un archivo para Inform 6 (internacional).
I7GB                     - Genera un archivo para Inform 7 (inglés).
I7SP                     - Genera un archivo para Inform 6 (español).
INFORM                   - Genera un archivo compilable por inform con librería inglesa
INFORMATE                - Genera un archivo compilable por inform con librería española
INPAWS                   - Genera un archivo para INPAWS, variante de SCE PAWS.
KENSHIRA                 - Genera un archivo para Kenshira.
LISTA                    - Lista localidades y objetos.
NULO                     - No hace nada.
PAWS                     - Genera un fichero SCE Paws.
SRC                      - Genera un archivo como el de entrada
STDC
SUPERGLUS                - Genera un archivo compilable por Superglús
inform
paws
```

El conjunto de *plugins* disponible varía según la versión, por lo que se aconseja utilizar --PL? para obtener los plugins exactos.
