// Declaración de procesos de la librería
// (Ver LibPAW.paw para una descripción más detallada)

PROCESS Salidas;    // Salidas obvias
                    // Para mostrar las salidas en cada descripción de localidad
                    // pon un #define MOSTRAR_SALIDAS en algún lugar de tu
                    // aventura.
PROCESS TermGenero; // Escribir 'o' 'a' 'os' u 'as' dependiendo del género del objeto actual (peso 1, 2, 3 o 4)
PROCESS TermGeneroVerbo; // Muestra 'n' si es plural
PROCESS ArtDet;     // Escribir articulo determinado del objeto actual (el, la, los, las), en minúscula
PROCESS ArtDetMayusc; // Lo mismo que el anterior en mayúscula
PROCESS ArtUndet;   // Escribir artículo indeterminado 'un' 'una' 'unos' 'unas', del objeto actual
PROCESS ProAcusativo;  // Escribir 'lo', 'la', 'los' o 'las'
PROCESS ProDativo;     // Escribir 'le' o 'les'
PROCESS CogerSub;   // Subrutina para coger objetos
PROCESS DejarSub;   // Subrutina para dejar objetos
PROCESS PonerSub;   // Subrutina para ponerse prendas
PROCESS QuitarSub;  // Subrutina para quitarse prendas
PROCESS ExaminarSub; // Examinar objetos
PROCESS AbrirSub;   // Abrir puertas, cerraduras
PROCESS CerrarSub;  // Cerrar cosas
PROCESS DarSub;     // Dar cosas a personajes
PROCESS MostrarSub; // Enseñar cosas a personajes
PROCESS MirarEnSub; // Mirar en los objetos contenedores
Process EstaCerrado; // Devuelve si un contenedor esta cerrado
PROCESS MeterSub; // Meter objetos dentro de otros
PROCESS SacarSub; // Sacar objetos de contenedores
Process GenNum; // Devuelve el genero y numero del objeto actual (1st noun)
Process PrintFirstArtDet; // Pinta el nombre del objeto precedido de artículo determinado
Process PrintFirstArtDetMayusc; // Lo mismo que el anterior pero comienza en mayuscula
Process PrintFirstArtUnDet; // Pinta el nombre del objeto precedido de artículo indeterminado
Process PrintSecond; // Muestra la descripción del objeto del second noun
Process PrintSecondArtDet; // Muestra el objeto del second noun precedido de artículo det.
Process PrintSecondArtUnDet; // Muestra el objeto del second noun precedido de artículo det.
Process PrintFirstDel;        // Muestra "del <first>" o "de la <first>" etc...
Process PrintFirstAl;         // Muestra "al <first>" o "a la <firsl>" etc...
Process PrintSecondDel;    // Muestra "del <second>" o "de la <second>" etc...
Process ContarAt;          // devuelve en flagRet el nº de objetos en la localidad flagPrm1
Process ListAt;            // Listado de objetos, devuelve el número de obj listados
Process ListAtFirstNoun;  // Lista los objetos que hay dentro del first noun
Process ListObj;        // Lista los objetos de la loc actual, si hay alguno
Process Inventario;     // Lista los objetos que tienes y los que llevas puestos
Process ExtPropiedad;   // Extender para definir propiedades extendidas 
                        // para algunos objetos en funcion de las cuales 
                        // la librería actuará de una forma u otra
Process DescPropiedad;  // Mensaje por defecto para objetos con alguna propiedad
                        // Si está abierto, lo que tiene dentro, etc.
Process AbrirText;      // Pinta "Abres el <first noun>"
Process CerrarText;     // Pinta "Cierras el <first noun>"
Process Present;        // Devuelve si está presente el first noun, teniendo
                        // en cuenta los contenedores (abiertos) que pueda haber
                        // presentes. Hasta el primer nivel sólamente
Process PresentSecond;  // Lo mismo para el second noun

// Constantes para definir género y número de los objetos
CONSTANT GenNum_MS 1; // Masc.Sing
CONSTANT GenNum_FS 2; // Fem.Sing
CONSTANT GenNum_MP 3; // Masc.Plur
CONSTANT GenNum_FP 4; // Fem.Plur
CONSTANT GenNum_NODEF 5; // Indefinido (se calculará por el peso)

// Propiedades que se pueden definir en ExtPropiedad
Flag PROP_GENNUM;   // Género y número del objeto
Flag PROP_NOLISTAR; // no listar el objeto en ListAt y ListObj
Flag PROP_ESTATICO; // No se puede coger
Flag PROP_ABRIBLE;  // Se puede abrir y cerrar (perdón por la licencia)
Flag PROP_ABIERTO;  // Está actualmente abierto
Flag PROP_CONTENEDOR; // Si el objeto es un contenedor (según el flag 56)
Flag PROP_INCONTABLE; // Para objetos que no requieren artículo indeterminado
                      // al listarlos (agua, arena, aceite,...)
