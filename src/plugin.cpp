// plugin.cpp
/*
        Lleva todo lo relacionado con los plugins

        baltasarq@yahoo.es
*/

#include "plugin.h"
#include "stringman.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <exception>
#include <stdexcept>

// ====================================================================== Plugin
Plugin::ListaPlugins Plugin::listaPlugins;
const std::string Plugin::StrLengua[] = {
    "DE", "EN", "ES", "FR", "IT", "NL", "SL", "SV", "Error"
};

const std::string Plugin::OpcLenguaInform[] = {
    "German", "English", "Spanish", "French", "Italian", "Dutch", "Slovenian", "Swedish", "Error"
};

const std::string &Plugin::lenguaACadena(Plugin::Lengua l)
{
    return StrLengua[ l ];
}

Plugin::Lengua Plugin::strALengua(const std::string &str)
{
    Plugin::Lengua toret = NoLanguage;
    std::string lng = StringMan::mays( StringMan::trim( str ) );

    for(size_t i = 0; i < NoLanguage; ++i ) {
        if ( StrLengua[ i ] == lng ) {
            toret = (Lengua) i;
            break;
        }
    }

    return toret;
}

const std::string &Plugin::lenguaAOpcionInform(Plugin::Lengua l)
{
    return OpcLenguaInform[ l ];
}

std::string &Plugin::procStr(std::string &d, char viejo, const std::string &nuevo)
{
        for(size_t i = 0; i < d.size(); ++i) {
                if ( d[ i ] == viejo ) {
                        d.erase( i, 1 );
                        d.insert( i, nuevo );
                        i += nuevo.size() - 1;
                }
        }

        return d;
}

bool Plugin::esObjetoInventario(TDS * tds, Objeto * obj)
{
    TDS::ListaInventarioObj::const_iterator pos = find(
                                tds->getListaInventarioObj()->begin(),
                                tds->getListaInventarioObj()->end(),
                                obj
    );

    return ( pos != tds->getListaInventarioObj()->end() );
}

Plugin * Plugin::buscaPlugin(const std::string &pl)
{
        Plugin * toret;
        ListaPlugins::const_iterator it = listaPlugins.find( pl );

        if ( it == listaPlugins.end() )
                toret = NULL;
        else    toret = it->second;

        return toret;
}

void Plugin::insertaEnListaPlugins(Plugin *pl)
{
        listaPlugins.insert( ListaPlugins::value_type( pl->getNombre(), pl ) );
}

void Plugin::iniciaListaPlugins()
{
        insertaEnListaPlugins( VoidPlugin::creaVoidPlugin() );
        insertaEnListaPlugins( ListaPlugin::creaListaPlugin() );
        insertaEnListaPlugins( HtmlPlugin::creaHtmlPlugin() );
        insertaEnListaPlugins( SrcPlugin::creaSrcPlugin() );
        insertaEnListaPlugins( InformatePlugin::creaInformatePlugin() );
        insertaEnListaPlugins( InformPlugin::creaInformPlugin() );
        insertaEnListaPlugins( I6i18nPlugin::creaI6i18nPlugin() );
        insertaEnListaPlugins( SuperglusPlugin::creaSuperglusPlugin() );
        insertaEnListaPlugins( PawsPlugin::creaPawsPlugin() );
        insertaEnListaPlugins( AGEPlugin::creaAGEPlugin() );
        insertaEnListaPlugins( GlulxI6i18nPlugin::creaGlulxI6i18nPlugin() );
        insertaEnListaPlugins( GlulxInformPlugin::creaGlulxInformPlugin() );
        insertaEnListaPlugins( GlulxInformatePlugin::creaGlulxInformatePlugin() );
        insertaEnListaPlugins( KenshiraPlugin::creaKenshiraPlugin() );
        insertaEnListaPlugins( I7SpPlugin::creaI7SpPlugin() );
        insertaEnListaPlugins( I7GbPlugin::creaI7GbPlugin() );
        insertaEnListaPlugins( BasPlugin::creaBasPlugin() );
        insertaEnListaPlugins( StdCPlugin::creaStdCPlugin() );
        insertaEnListaPlugins( FiJsPlugin::creaFiJsPlugin() );
}


Plugin::Plugin(const std::string &a, const std::string &n, const std::string &v, const std::string &h)
        : nombrePlugin(n), autorPlugin(a), versionPlugin(v), helpPlugin(h), lengua(ES)
{
        StringMan::normCnvt( nombrePlugin );
}

Plugin::~Plugin()
{
}

std::string Plugin::getInfo() const
{
        return  getNombre() + ' ' + getVersion() + " (c) " + getAutor() + '\n'
              + getHelp()
        ;
}

// ================================================================== VoidPlugin
const std::string VoidPlugin::Nombre = "NULO";

VoidPlugin::VoidPlugin(const std::string &a, const std::string &n, const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

VoidPlugin * VoidPlugin::creaVoidPlugin()
{
        VoidPlugin * toret = new(std::nothrow) VoidPlugin(
                                "Baltasar", Nombre, "v0.1",
                                "Este plugin no hace absolutamente nada, "
                                "es el plugin que se ejecuta por defecto."
        );

        if ( toret == NULL ) {
            throw ErrorInterno( "Sin memoria, creando plugin nulo" );
        }

        return toret;
}

void VoidPlugin::procesar(TDS *)
{
        nomFichSal.clear();
        porPantalla( "No se ha pedido tarea alguna ... fin.\n\n" );
}

// ================================================================= ListaPlugin
ListaPlugin::ListaPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

ListaPlugin * ListaPlugin::creaListaPlugin()
{
        ListaPlugin * toret = new(std::nothrow) ListaPlugin(
                                "Baltasar", "Lista", "v0.1",
                                "Este plugin lista los nombres de las "
                                "localidades, y sus conexiones."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin lista" );
        }

        return toret;
}

void ListaPlugin::procesar(TDS *tds)
{
        std::ostringstream cnvt;

        nomFichSal = "<stdin>";

        // Localidades
        cnvt << tds->nombreAventura << ": num. de localidades: " << tds->getNumLocs();

        porPantalla( cnvt.str() );
        porPantalla( "\n---\n\n" );

        Localidad * loc = tds->getPriLoc();

        while( !tds->esLocalidadFinal() )
        {
                porPantalla( loc->toString() );
                porPantalla( "\n\n" );
                loc = tds->getSigLoc();
        }

        // Objetos
        cnvt.str( "" );
        cnvt << "Num. de objetos: " << tds->getNumObjs();

        porPantalla( cnvt.str() );
        porPantalla( "\n---\n\n" );

        Objeto * obj = tds->getPriObj();

        while( !tds->esObjetoFinal() )
        {
                porPantalla( obj->toString() );
                porPantalla( "\n\n" );
                obj = tds->getSigObj();
        }
}

// ================================================================== HtmlPlugin
HtmlPlugin::HtmlPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

HtmlPlugin * HtmlPlugin::creaHtmlPlugin()
{
        HtmlPlugin * toret = new(std::nothrow) HtmlPlugin(
                                "Baltasar", "Html", "v0.3",
                                "Este plugin crea una serie de archivos HTML "
                                "que permiten probar las conexiones entre las "
                                "localidades."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin html" );
        }

        return toret;
}

std::string &HtmlPlugin::cnvtStrHtml(std::string &str)
{
        procStr( str, '\n', "<br>" );
        procStr( str, 'á', "&aacute;" );
        procStr( str, 'é', "&eacute;" );
        procStr( str, 'í', "&iacute;" );
        procStr( str, 'ó', "&oacute;" );
        procStr( str, 'ú', "&uacute;" );
        procStr( str, 'ñ', "&ntilde;" );

        return str;
}

std::string HtmlPlugin::cnvtStrHtml(const std::string &str)
{
	std::string toret = str;

	return cnvtStrHtml( toret );
}

void HtmlPlugin::procesar(TDS *tds)
{
        FILE * f;
        FILE * findex;
        FILE * fmenu;
        Localidad * loc;
        Localidad * locDest;
        Objeto * obj;
        std::ostringstream cnvt;
        std::string nombreLocDest;
        std::string nombreFichLoc;
        int numSalidas;
        std::string hora = StringMan::toString( time( NULL ) );

        // Crear archivo index
        porPantalla( "\nCreando index.html" );
        findex = fopen( ( nomFichSal + "index.html" ).c_str(), "wt" );
        if ( findex != NULL ) {
            fprintf( findex, "<!DOCTYPE HTML PUBLIC \""
                        "-//W3C//DTD HTML 4.01 Transitional//EN\">"
                        "\n<html>\n<head>\n\n"
            );

            fprintf( findex, "<meta http-equiv=\"Content-Type\" content"
                        "=\"text/html; charset=iso-8859-1\">\n\n"
            );

            fprintf( findex, "<title>%s</title>\n\n", tds->nombreAventura.c_str() );

            fprintf( findex, "<frameset cols=\"33%%,*\"><frame name=\"menu\" "
                        "src=\"menu.html\" frameborder=\"1\" scrolling=\"yes\">"
                        "<frame name=\"contents\" "
                        "src=\"%s\" frameborder=\"0\" scrolling=\"yes\"></frameset>"
                        "\n\n</html>",
                        ( StringMan::mins( tds->locComienzo->getIdUnico() ) + ".html" ).c_str()
            );

            fclose( findex );
        }
        else porPantalla( "\nError creando index.html\n" );

        // Crear archivo menu
        porPantalla( "\nCreando menu.html" );
        fmenu = fopen( ( nomFichSal + "menu.html" ).c_str(), "wt" );
        if ( fmenu != NULL ) {
            fprintf( fmenu, "<!DOCTYPE HTML PUBLIC \""
                        "-//W3C//DTD HTML 4.01 Transitional//EN\">"
                        "\n<html>\n<head>\n\n"
            );

            fprintf( fmenu, "<meta http-equiv=\"Content-Type\" content"
                        "=\"text/html; charset=iso-8859-1\">\n\n</head>\n\n"
            );

            fprintf( fmenu, "<body>\n\n<table width=\"100%%\"" );
            fprintf( fmenu, "<tr>\n\t<td style=\"text-align: center;\" width=\"100%%\">"
                        "%s</td>\n</tr>\n\n", tds->nombreAventura.c_str()
            );

            int i = 0;
            for(loc = tds->getPriLoc(); !tds->esLocalidadFinal(); loc = tds->getSigLoc(), ++i ) {
                fprintf( fmenu, "<tr>\n\t<td style=\"background-color: %s; "
                            "text-align: center;\" width=\"100%%\">"
                            "<a href=\"%s\" target=\"contents\">%s</a></td>\n</tr>\n\n",
                        ( i % 2 )== 0 ? "rgb(160, 202, 242)" : "rgb(255, 255, 255)",
                        ( StringMan::mins( loc->getIdUnico() ) + ".html" ).c_str(),
                        loc->getId().c_str()
                );
            }

            fclose( fmenu );
        }
        else porPantalla( "\nError creando menu.html\n" );

        // Crear html's de localidades
        loc = tds->getPriLoc();
        while( !tds->esLocalidadFinal() )
        {
                nombreFichLoc = nomFichSal;
                nombreFichLoc += StringMan::mins( loc->getIdUnico() );
                f = fopen( ( nombreFichLoc + ".html" ).c_str(), "wt" );

                if ( f != NULL )
                {
                    // Crear archivo localidad
                    porPantalla( "\nCreando " + nombreFichLoc + ".html" );


                    fprintf( f, "<!-- generado por %s@%s, %s/%s\n! %s-->\n\n\n",
                                cnvtStrHtml( getNombre() ).c_str(),
                                cnvtStrHtml( nombre ).c_str(),
                                cnvtStrHtml( getVersion() ).c_str(),
                                cnvtStrHtml( version ).c_str(),
                                cnvtStrHtml( hora ).c_str()
                    );

                    // Título
                    fprintf( f,
                             "<html><head><title>%s</title></head><body>",
                             cnvtStrHtml( loc->getId() ).c_str()
                    );

                    // Grafico
                    if ( !( loc->nombreRecGrafico.empty() ) ) {
                        fprintf( f, "\n\n<img src=\"%s\">\n\n", loc->nombreRecGrafico.c_str() );
                    }

                    // Desc
                    fprintf( f, "\n<h1>%s</h1><P>", loc->getId().c_str() );
                    fprintf( f, "\n%s\n<P>\n",
                                cnvtStrHtml( loc->getDesc() ).c_str()
                    );
                    if ( !( loc->getComentario().empty() ) ) {
                        fprintf( f, "\n<p><i>%s</i><p>",
                                cnvtStrHtml( loc->getComentario() ).c_str()
                        );
                    }

                    // Objetos
                    if ( loc->getNumObjs() > 0 )
                    {
                        fprintf( f, "\n<ul>Objetos:");
                        obj = loc->getPriObj();
                        for(; !loc->esObjetoFinal(); obj = loc->getSigObj())
                        {
                            fprintf( f, "\n<li><b>%s</b>%s(%s)",
                                        cnvtStrHtml( obj->getId() ).c_str(),
                                        obj->esEscenario() ? "[stc]" : "",
                                        cnvtStrHtml( obj->getDesc() ).c_str()
                            );
                        }

                        fprintf( f, "\n</ul>\n" );
                    }

                    // Salidas
                    fprintf( f, "\n<P><ul>Salidas:" );

                    numSalidas = 0;
                    for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                    {
                        nombreLocDest = (loc->getSalidas())[i];
                        if ( !nombreLocDest.empty() ) {
                                ++numSalidas;
                                locDest = tds->buscaLoc( nombreLocDest );

                                if ( locDest == NULL ) {
                                        throw ErrorInterno(
                                                nombreLocDest
                                                + ": no encontrado(?)" )
                                        ;

                                }

                                nombreLocDest = StringMan::mins(
                                                        locDest->getIdUnico()
                                );

                                fprintf( f,
                                     "\n<li><a href=\"%s\"><b>%s</b> "
                                     "hacia %s</a>",
                                     ( nombreLocDest + ".html" ).c_str(),
                                     Localidad::strDireccion[i].c_str(),
                                     cnvtStrHtml( locDest->getId() ).c_str()
                                );
                        }
                    }

                    if ( numSalidas == 0 ) {
                            fprintf( f, "\n<li><b>Ninguna</b>." );
                    }

                    fprintf( f, "\n</ul><P></body></html>" );
                } else throw ErrorES( "creando " + nombreFichLoc + ".html\n" );

                fclose( f );

                loc = tds->getSigLoc();
        }
        porPantalla( "\nHTML creado\n" );
}

// =================================================================== SrcPlugin
SrcPlugin::SrcPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

SrcPlugin * SrcPlugin::creaSrcPlugin()
{
        SrcPlugin * toret = new(std::nothrow) SrcPlugin(
                                "Baltasar", "Src", "v0.2",
                                "Este plugin genera un archivo que puede "
                                "ser utilizado como entrada para txtMap"
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin src" );
        }

        return toret;
}

void SrcPlugin::procesaLocalidad(FILE *f, TDS * tds, Localidad * loc)
{
    // Guardar el encabezado
    fprintf( f, "%s\n\n", loc->getId().c_str() );

    // Guardar recursos
    if ( !loc->nombreRecGrafico.empty() ) {
            fprintf( f, ".grf \"%s\"\n",
                loc->nombreRecGrafico.c_str()
            );
    }

    if ( !loc->nombreRecMusica.empty() ) {
            fprintf( f, ".msc \"%s\"\n",
                loc->nombreRecMusica.c_str()
            );
    }

    // Guardar la desc.
    fprintf( f, "\n%s\n\n", loc->getDesc().c_str() );

    // Guardar el comentario (si existe)
    if ( !loc->getComentario().empty() ) {
            fprintf( f, "\n[%s]\n\n", loc->getComentario().c_str() );
    }

    // Objetos de inventario
    if ( loc == tds->locComienzo
      && tds->getListaInventarioStr()->size() > 0 )
    {
            fprintf( f, "> i\n" );
            for(size_t i = 0; i < tds->getListaInventarioStr()->size(); ++i) {
                if ( i > 0 ) {
                    fprintf( f, ",\n" );
                }

                fprintf( f, "%s", ( *( tds->getListaInventarioStr() ) )[ i ].c_str() );
            }

            fprintf( f, "\n\n" );
    }

    // Guardar los objetos correspondientes
    for(Objeto * obj = loc->getPriObj(); !( loc->esObjetoFinal() ); obj = loc->getSigObj() ) {
        fprintf( f, "> ex %s\n%s\n\n", obj->getId().c_str(), obj->getDesc().c_str() );

        if ( ! ( obj->esEscenario() ) ) {
            fprintf( f, "> coge %s\nCogida.\n\n", obj->getId().c_str() );
        }

        if ( obj->esPonible() ) {
            fprintf( f, "> ponte %s\nHecho.\n\n", obj->getId().c_str() );
        }
    }

    // Guardar las direcciones
    for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
    {
            if ( !((loc->getSalidas())[i]).empty() ) {
                fprintf( f, "> %s a %s\n",
                         Localidad::strDireccion[i].c_str(),
                         (loc->getSalidas())[i].c_str()
                );
            }
    }

    fprintf( f, "\n.finloc\n\n" );
}

void SrcPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        std::ostringstream cnvt;
        std::string hora = StringMan::toString( time( NULL ) );

        // Crear el archivo
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".txt" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        fprintf( f, "! generado por %s@%s, %s/%s\n! %s\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Primera localidad
        loc = tds->locComienzo;
        procesaLocalidad( f, tds, loc );

        // Localidades
        loc = tds->getPriLoc();

        while( !tds->esLocalidadFinal() )
        {
                if ( loc != tds->locComienzo ) {
                    procesaLocalidad( f, tds, loc );
                }

                loc = tds->getSigLoc();
        }

        fclose( f );
}

// =================================================================== InfPlugin
const std::string InfPlugin::Jugador = "player";
const std::string InfPlugin::strIdsInformate[] = {
        "al_n",
        "al_s",
        "al_e",
        "al_o",
        "arriba",
        "abajo",
        "al_ne",
        "al_so",
        "al_no",
        "al_se",
        "adentro",
        "afuera",
        "\nInclude \"EParser\";"
                "\nInclude \"infglk\";"
                "\nInclude \"smw\";"
                "\n\n%s\n"
                "\nInclude \"Acciones\";"
                 "\nInclude \"Gramatica\";\n\n",
        "\nInclude \"EParser\";"
                "\nInclude \"infglk\";"
                "\nInclude \"smw\";"
                "\nInclude \">%s.bli\";"
                "\n\n%s\n"
                "\nInclude \"Acciones\";"
                 "\nInclude \"Gramatica\";\n\n",
        "inicializar",
        "localizacion",
        "descripcion",
        "luz",
        "nombre",
        "escenario",
        "femenino",
        "nombreplural",
        "prenda"
};

const std::string InfPlugin::strIdsInform[] = {
        "n_to",
        "s_to",
        "e_to",
        "w_to",
        "u_to",
        "d_to",
        "ne_to",
        "sw_to",
        "nw_to",
        "se_to",
        "in_to",
        "out_to",
        "\nInclude \"Parser\";"
                "\nInclude \"infglk\";"
                 "\nInclude \"smw\";"
                 "\n\n%s\n"
                 "\nInclude \"VerbLib\";"
                 "\nInclude \"Grammar\";\n\n",
        "\nInclude \"Parser\";"
                "\nInclude \"infglk\";"
                "\nInclude \"smw\";"
                "\nInclude \">%s.bli\";"
                "\n\n%s\n"
                "\nInclude \"VerbLib\";"
                 "\nInclude \"Grammar\";\n\n",
        "initialise",
        "location",
        "description",
        "light",
        "name",
        "scenery",
        "",
        "",
        "clothing"
};

const std::string InfPlugin::strIdsI6i18n[] = {
        "n_to",
        "s_to",
        "e_to",
        "w_to",
        "u_to",
        "d_to",
        "ne_to",
        "sw_to",
        "nw_to",
        "se_to",
        "in_to",
        "out_to",
        "\nInclude \"Parser\";"
                "\nInclude \"infglk\";"
                 "\nInclude \"smw\";"
                 "\n\n%s\n"
                 "\nInclude \"VerbLib\";"
                 "\nInclude \"%s\";\n\n",
        "\nInclude \"Parser\";"
                "\nInclude \"infglk\";"
                "\nInclude \"smw\";"
                "\nInclude \">%s.bli\";"
                "\n\n%s\n"
                "\nInclude \"VerbLib\";"
                 "\nInclude \"%s\";\n\n",
        "initialise",
        "location",
        "description",
        "light",
        "name",
        "scenery",
        "female",
        "pluralname",
        "clothing"
};

const std::string InfPlugin::Recurso::EtqGrf = "grf";
const std::string InfPlugin::Recurso::EtqMsc = "msc";
const std::string InfPlugin::Recurso::EtqCodeMark = "CODE";
const std::string InfPlugin::Recurso::EtqPictureMark = "PICTURE";
const std::string InfPlugin::Recurso::EtqMusicMark = "SOUND";

const std::string InfPlugin::ResExt = ".res";
const std::string InfPlugin::UlxExt = ".ulx";
const std::string InfPlugin::ExtInf = ".inf";
const std::string InfPlugin::EtqThing = "Thing";
const std::string InfPlugin::EtqSceneryThing = "SceneryThing";
const std::string InfPlugin::EtqRoom = "Room";

std::string InfPlugin::getNombreArchivo(const std::string &nf)
{
    if ( nombreArchivo.empty() ) {
        std::string nombre = StringMan::extraerArchivoSinExtDeNombreArchivo( nf );

        return nombre;
    }
    else return nombreArchivo;
}

void InfPlugin::generarArchivoRes(const std::string &nf)
{
    std::ostringstream contenido;
    std::string nombreArchivo = getNombreArchivo( nf );
    std::string nombreArchivoRes = nombreArchivo + ResExt;

    contenido << Recurso::EtqCodeMark << ' ' << nombreArchivo << UlxExt << std::endl;

    // Recorrer la lista de recursos e incorporarlas al contenido
    ListaRecursos::const_iterator it = recursos.begin();
    for(; it != recursos.end(); ++it) {
        contenido << it->getMark() << ' ' << it->getIdRecurso()
                  << ' ' << it->archivoRecurso << std::endl
        ;
    }

    // Crear el archivo res
    FILE * f = fopen( nombreArchivoRes.c_str(), "wt" );

    if ( f != NULL ) {
        fprintf( f, "%s", contenido.str().c_str() );
        fclose( f );
    }
    else throw std::runtime_error( "f?: " + nombreArchivoRes );
}

InfPlugin::InfPlugin(const std::string &a, const std::string &n,
                     const std::string &v, const std::string &h,
                     bool grf, bool infate, bool i18n)
        : Plugin(a, n, v, h), ids(strIdsInform), conGraficos(grf),
          _esInformATE(infate), _esI18n(i18n)
{
        icl = "!% -SD";

        if ( esInformATE() ) {
            jugador = &InformatePlugin::Jugador;
            ids = strIdsInformate;
            classRoom = InformatePlugin::EtqLocalidad;
            classThing = InformatePlugin::EtqObjeto;
            classSceneryThing = InformatePlugin::EtqObjetoEstatico;
            fnFinal = InformatePlugin::FinalCorrecto;
        } else {
            jugador = &Jugador;
            classRoom = EtqRoom;
            classThing = EtqThing;
            classSceneryThing = EtqSceneryThing;
            fnFinal = InformPlugin::FinalCorrecto;
        }
}

void InfPlugin::procesar(TDS *tds)
{
        Localidad * locIni = tds->locComienzo;
        Localidad * loc;
        Objeto *obj;
        std::string aux;
        std::string objetosInventario;
        std::string hora = StringMan::toString( time( NULL ) );
        std::string mediaCmd;
        std::string nombreArchivo;
        std::string includeLengua = ( lenguaAOpcionInform( lengua ) + "G" );

        // Abrir la salida
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ExtInf );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar ICL
        fprintf( f, "%s\n", icl.c_str() );

        // Guardar la cabecera
        fprintf( f, "\n! generado por %s@%s, %s/%s\n! %s\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Constantes
        if ( esInformATE() ) {
                fprintf( f, "\nConstant ADMITIR_COMANDO_SALIDAS;\n" );
                fprintf( f, "Constant Historia \"%s\";\n", tds->nombreAventura.c_str() );
                fprintf( f, "Constant Titular \"^Un relato interactivo.^^\";\n\n\n" );
        } else {
                fprintf( f, "Constant Story \"%s\";\n", tds->nombreAventura.c_str() );
                fprintf( f, "Constant Headline \"^An interactive fiction piece.^^\";\n\n\n" );
        }

        // Curiosidades
        if ( !esInformATE() )
                fprintf(f, "Constant AMUSING_PROVIDED;\n[ amusing;\n" );
        else    fprintf(f, "Constant HAY_CURIOSIDADES;\n[ curiosidades;\n" );

        fprintf( f, "\tprint \"generado por %s@@64%s, %s/%s - %s\";\n];\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Includes
        nombreArchivo = StringMan::extraerNombreArchivoSinPathNiExt( nomFichSal ).c_str();
        if ( esI18n() ) {
            if ( esGrafico() )
                    fprintf( f, ids[ includesGraf ].c_str(),
                                nombreArchivo.c_str(),
                                fnFinal.c_str(),
                                includeLengua.c_str()
                    );
            else    fprintf( f, ids[ includes ].c_str(),
                                fnFinal.c_str(),
                                includeLengua.c_str()
                    );
        } else {
            if ( esGrafico() )
                    fprintf( f, ids[ includesGraf ].c_str(),
                                nombreArchivo.c_str(),
                                fnFinal.c_str()
                    );
            else    fprintf( f, ids[includes].c_str(), fnFinal.c_str() );
        }

        // El limbo y las clases
        fprintf( f,
                 "object Limbo \"Limbo\";\n"
                 "\nclass %s\nhas %s;\n\n"
                 "\n\nclass %s;\n"
                 "\nclass %s\nclass %s\nhas %s;\n\n"
                 ,
                 classRoom.c_str(),
                 ids[luz].c_str(),
                 classThing.c_str(),
                 classSceneryThing.c_str(),
                 classThing.c_str(),
                 ids[escenario].c_str()
        );

        // Fn init
        for(size_t i = 0; i < tds->getListaInventarioObj()->size(); ++i) {
                objetosInventario += "\n\tmove "
                  + ( *( tds->getListaInventarioObj() ) )[ i ]->getIdUnico()
                  + " to "
		  + ( *jugador ) + ";"
                ;
        }

        fprintf( f,
                 "\n[ %s;\n\t%s = %s;",
                 ids[init].c_str(),
                 ids[local].c_str(),
                 locIni != NULL ? locIni->getIdUnico().c_str() : "limbo"
        );

        // Objetos de inventario
        if ( !objetosInventario.empty() ) {
                fprintf( f, "\n\n\t! Objetos de inventario%s",
                         objetosInventario.c_str()
                );
        }

        // Parte de init grf
        if ( esGrafico() ) {
                fprintf( f,
                      "\n\n\t! Grfs & Snds ---"
                      "\n\tinitializeSMW( 240 );\n"
                );
        }

        fprintf( f, "\n];\n" );

        fprintf ( f, "\n! ================= %ss ===============\n\n", classRoom.c_str() );

        loc = tds->getPriLoc();

        while( !tds->esLocalidadFinal() )
        {
                // Guardar el comentario (si existe)
                if ( !loc->getComentario().empty() ) {
                        fprintf( f, "\n! [%s]\n", loc->getComentario().c_str() );
                }

                // Guardar el título
                fprintf( f, "\%s %s \"%s\"\nwith\n",
                        classRoom.c_str(),
                        loc->getIdUnico().c_str(),
                        loc->getId().c_str()
                );

                mediaCmd = "";
                if ( esGrafico() ) {
                        // Guardar el grf de la localidad
                        if ( !loc->nombreRecGrafico.empty() ) {
                                Recurso r;
                                r.idLoc = loc->getIdUnico();
                                r.archivoRecurso = loc->nombreRecGrafico;
                                r.tipoRecurso = Recurso::Grf;
                                mediaCmd += "\n\t\tviewImageCenter( " + r.getIdRecurso() + " );";
                                recursos.push_back( r );
                        }
                        // Guardar la msc de la localidad
                        if ( !loc->nombreRecMusica.empty() ) {
                                Recurso r;
                                r.idLoc = loc->getIdUnico();
                                r.archivoRecurso= loc->nombreRecMusica;
                                r.tipoRecurso = Recurso::Msc;
                                mediaCmd += "\n\t\tplaySound( chan1, "
                                                + r.getIdRecurso()
                                                + ", 1, VOLUME_NORMAL );"
                                ;
                                recursos.push_back( r );
                        }

                        if ( !mediaCmd.empty() ) {
                            mediaCmd += '\n';
                        }
                }

                // Guardar la desc.
                fprintf( f, "\t%s [; %s\t\tprint \"%s^\";\n\t\trtrue;\n\t],\n",
                        ids[desc].c_str(),
                        mediaCmd.c_str(),
                        ( StringMan::cambiarCaracteres(
                            StringMan::cambiarCaracteres(
                                loc->getDesc(), '"', '~'
                            ), '\n', '^' )
                        ).c_str()
                );

                // Guardar las direcciones
                for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                {
                        if ( !((loc->getSalidas())[i]).empty() ) {
                            fprintf( f, "\t%s %s,\n",
                                     ids[i].c_str(),
                                     Localidad::cnvtId(
                                                (loc->getSalidas())[i] ).c_str()
                            );
                        }
                }

                fprintf( f, "\n;\n" );

                loc = tds->getSigLoc();
        }

        fprintf ( f, "\n! ================= %ss ===============\n\n", classThing.c_str() );

        obj = tds->getPriObj();
        while( !tds->esObjetoFinal() )
        {
                // Escribir la info del objeto
                fprintf( f, "\n%s %s \"%s\" %s",
                        ( obj->esEscenario() ? classSceneryThing.c_str() : classThing.c_str() ),
                        obj->getIdUnico().c_str(),
                        obj->getId().c_str(),
                        obj->getContinente()->getIdUnico().c_str()
                );

                fprintf( f, "\nwith\n\t%s '%s',\n\t%s \"%s\"\nhas %s %s %s;\n",
                        ids[name].c_str(),
                        obj->getNomVoc().c_str(),
                        ids[desc].c_str(),
                        ( StringMan::cambiarCaracteres(
                            StringMan::cambiarCaracteres(
                                obj->getDesc(), '"', '~'
                            ), '\n', '^' )
                        ).c_str(),
                        obj->esFemenino() ? ids[femenino].c_str() : "",
                        obj->esPlural() ? ids[plural].c_str() : "",
                        obj->esPonible() ? ids[ponible].c_str() : ""
                );

                obj = tds->getSigObj();
        }

        fclose( f );

        if ( esGrafico() ) {
            generarArchivoRes( nomFichSal );
        }
}

// ------------------------------------------------------------- InformatePlugin
const std::string InformatePlugin::Jugador = "jugador";
const std::string InformatePlugin::EtqObjeto = "Objeto";
const std::string InformatePlugin::EtqObjetoEstatico = "ObjetoEstatico";
const std::string InformatePlugin::EtqLocalidad = "Estancia";
const std::string InformatePlugin::FinalCorrecto =
    "Object MensajesLibreria\n"
    "with\n\tantes [;\n\t\tMiscelanea:\n\t\t\tif ( ml_n == 4 ) {\n"
    "\t\t\t\tprint \"¡Has ganado! ***^\";\n"
    "\t\t\t\trtrue;\n"
    "\t\t\t}\n"
    "\t]\n;"
;

InformatePlugin::InformatePlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h, false, true)
{
}

InformatePlugin * InformatePlugin::creaInformatePlugin()
{
        InformatePlugin * toret = new(std::nothrow) InformatePlugin(
                                "Baltasar", "Informate", "v0.3",
                                "Este plugin genera el fuente para las "
                                "localidades en InformATE!, listo para ser "
                                "consumido por el compilador."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin InformATE!" );
        }

        return toret;
}

// ------------------------------------------------------------ GlulxI6i18nPlugin
GlulxI6i18nPlugin * GlulxI6i18nPlugin::creaGlulxI6i18nPlugin()
{
        GlulxI6i18nPlugin * toret = new(std::nothrow) GlulxI6i18nPlugin(
                                "Baltasar", "GlulxI6i18n", "v0.3",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para "
                                "I6i18n con Glulx (mediante SMW)."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin glulx" );
        }

        return toret;
}

GlulxI6i18nPlugin::GlulxI6i18nPlugin(const std::string &a, const std::string &n,
                                 const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h, true, false, true)
{
        ids = strIdsI6i18n;
}

void GlulxI6i18nPlugin::procesar(TDS * tds)
{
    icl = "!% -SD\n!% +language_name=" + lenguaAOpcionInform( lengua );

    InfPlugin::procesar( tds );
}

// ---------------------------------------------------------- GlulxInformPlugin
GlulxInformPlugin * GlulxInformPlugin::creaGlulxInformPlugin()
{
        GlulxInformPlugin * toret = new(std::nothrow) GlulxInformPlugin(
                                "Baltasar", "GlulxI6Gb", "v0.3",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para "
                                "I6Gb con Glulx (mediante SMW)."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin glulx" );
        }

        return toret;
}

GlulxInformPlugin::GlulxInformPlugin(const std::string &a, const std::string &n,
                                     const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h, true)
{
}

// ------------------------------------------------------- GlulxInformatePlugin
GlulxInformatePlugin * GlulxInformatePlugin::creaGlulxInformatePlugin()
{
        GlulxInformatePlugin * toret = new(std::nothrow) GlulxInformatePlugin(
                                "Baltasar", "GlulxInformATE!", "v0.3",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para "
                                "InformATE! con Glulx (mediante SMW)."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin glulx" );
        }

        return toret;
}

GlulxInformatePlugin::GlulxInformatePlugin(const std::string &a, const std::string &n,
                                           const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h, true, true)
{
}

// ---------------------------------------------------------------- InformPlugin
const std::string InformPlugin::FinalCorrecto =
    "Object LibraryMessages\n"
    "with\n\tbefore [;\n\t\tMiscellany:\n\t\t\tif ( lm_n == 4 ) {\n"
    "\t\t\t\tprint \"¡Has ganado! ***^\";\n"
    "\t\t\t\trtrue;\n"
    "\t\t\t}\n"
    "\t]\n;"
;

InformPlugin::InformPlugin(const std::string &a, const std::string &n,
                           const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h)
{
    ids = const_cast<std::string *>( strIdsInform );
}

InformPlugin * InformPlugin::creaInformPlugin()
{
        InformPlugin * toret = new(std::nothrow) InformPlugin(
                                "Baltasar", "Inform", "v0.3",
                                "Este plugin genera el fuente para las "
                                "localidades en Inform, listo para ser "
                                "consumido por el compilador."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin inform" );
        }

        return toret;
}

// ---------------------------------------------------------------- I6i18nLugin
I6i18nPlugin::I6i18nPlugin(const std::string &a, const std::string &n,
                           const std::string &v, const std::string &h)
        : InfPlugin(a, n, v, h, false, false, true)
{
    ids = const_cast<std::string *>( strIdsI6i18n );
}

I6i18nPlugin * I6i18nPlugin::creaI6i18nPlugin()
{
        I6i18nPlugin * toret = new(std::nothrow) I6i18nPlugin(
                                "Baltasar", "I6i18n", "v0.3",
                                "Este plugin genera el fuente para las "
                                "localidades en Inform, listo para ser "
                                "consumido por el compilador."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin inform" );
        }

        return toret;
}

void I6i18nPlugin::procesar(TDS * tds)
{
    icl = "!% -SD\n!% +language_name=" + lenguaAOpcionInform( lengua );

    InfPlugin::procesar( tds );
}

// ============================================================= SuperglusPlugin
SuperglusPlugin::SuperglusPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

SuperglusPlugin * SuperglusPlugin::creaSuperglusPlugin()
{
        SuperglusPlugin * toret = new(std::nothrow) SuperglusPlugin(
                                "Baltasar", "superglus", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para superglus"
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin superglus" );
        }

        return toret;
}

void SuperglusPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        std::ostringstream cnvt;
        std::string hora = StringMan::toString( std::time( NULL ) );

        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".txp" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        fprintf( f, "; generado por %s@%s, %s/%s\n! %s\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Guardar las descripciones de las localidades
        loc = tds->getPriLoc();

        fprintf( f, "\n/LTX\n" );

        while( !tds->esLocalidadFinal() )
        {
                // Guardar el encabezado
                fprintf( f, "\n/%u ;%s\n",
                        loc->getNumLoc(),
                        loc->getId().c_str()
                );

                fprintf( f, "\n#define loc %s = %u\n",
                        ( std::string( "loc" ) + loc->getIdUnico() ).c_str(),
                        loc->getNumLoc()
                );

                // Recursos de msc. y sonido
                if ( !loc->nombreRecGrafico.empty() ) {
                        fprintf( f, "\n#define pic %s = %u\n",
                                loc->nombreRecGrafico.c_str(),
                                loc->getNumLoc()
                        );
                }
                if ( !loc->nombreRecMusica.empty() ) {
                        fprintf( f, "\n#define msc %s = %u\n",
                                loc->nombreRecMusica.c_str(),
                                loc->getNumLoc()
                        );
                }

                // Guardar la desc.
                fprintf( f, "%s\n",
                        procStr( loc->getDesc(), '\n', "\n\n" ).c_str()
                );

                // Guardar el comentario (si existe)
                if ( !loc->getComentario().empty() ) {
                        fprintf( f, "\t;[%s]\n", loc->getComentario().c_str() );
                }

                loc = tds->getSigLoc();
        }

        // Guardar las conexiones
        loc = tds->getPriLoc();

        fprintf( f, "\n/CON\n" );

        while( !tds->esLocalidadFinal() )
        {
                fprintf( f, "\n/%s\n", loc->getIdUnico().c_str() );

                // Guardar las direcciones
                for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                {
                        if ( !((loc->getSalidas())[i]).empty() ) {
                            fprintf( f, "%s %s\n",
                                Localidad::strDireccion[i].c_str(),
                                ( std::string( "loc" )
                                    + Localidad::cnvtId(
                                             (loc->getSalidas())[i] ) ).c_str()
                            );
                        }
                }

                loc = tds->getSigLoc();
        }

        fclose( f );
}

// ================================================================== PawsPlugin
PawsPlugin::PawsPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

PawsPlugin * PawsPlugin::creaPawsPlugin()
{
        PawsPlugin * toret = new(std::nothrow) PawsPlugin(
                                "Baltasar", "paws", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para el "
                                "compilador SCE PAWS."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin paws" );
        }

        return toret;
}

void PawsPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        Localidad * locCon;
        std::string aux;
        std::string hora = StringMan::toString( std::time( NULL ) );

        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".sce" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        fprintf( f, "; generado por %s@%s, %s/%s\n! %s\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Guardar las descripciones de las localidades
        loc = tds->getPriLoc();

        fprintf( f, "\n/LTX\n" );

        while( !tds->esLocalidadFinal() )
        {
                // Guardar el encabezado
                fprintf( f, "\n/%u ;%s\n",
                        loc->getNumLoc(),
                        loc->getId().c_str()
                );

                // Guardar la desc.
                fprintf( f, "%s\n",
                        procStr( loc->getDesc(), '\n', "\n\n" ).c_str()
                );

                // Guardar el comentario (si existe)
                if ( !loc->getComentario().empty() ) {
                        fprintf( f, "\t;[%s]\n", loc->getComentario().c_str() );
                }

                loc = tds->getSigLoc();
        }

        // Guardar las conexiones
        loc = tds->getPriLoc();

        fprintf( f, "\n/CON\n" );

        while( !tds->esLocalidadFinal() )
        {
                fprintf( f, "\n/%u\n", loc->getNumLoc() );

                // Guardar las direcciones
                for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                {
                        if ( !((loc->getSalidas())[i]).empty() ) {
                              locCon = tds->buscaLoc( (loc->getSalidas())[i] );

                              if ( locCon == NULL ) {
                                      throw ErrorInterno( "no encontrado '"
                                                      + (loc->getSalidas())[i]
                                                      + '\''
                                      );
                              }


                              fprintf( f, "%s %u\n",
                                      Localidad::strDireccion[i].c_str(),
                                      locCon->getNumLoc()
                              );
                        }
                }

                loc = tds->getSigLoc();
        }

        // Guardar los mensajes de los objetos
        fprintf( f, "\n\n/OTX" );

        Objeto * obj = tds->getPriObj();

        while( !tds->esObjetoFinal() )
        {
                fprintf( f, "\n/%d\n%s\n",
                        obj->getNumObj(),
                        obj->getDesc().c_str()
                );

                obj = tds->getSigObj();
        }

        // Guardar la info de los objetos
        fprintf( f, "\n\n/OBJ" );
        fprintf( f, "\n; #obj\tstart\tweight\tcont.\twearbl.\tnoun\tadj\n" );

        obj = tds->getPriObj();

        while( !tds->esObjetoFinal() )
        {
                // Num. de objeto
                fprintf( f, "\n/%d\t", obj->getNumObj() );

                // Localidad/Llevado/Puesto y peso
                aux.erase();
                if ( obj->esLlevado() ) {
                        if ( obj->esPonible() )
                                aux = "WORN";
                        else    aux = "CARRIED";
                }
                else {
                        aux = StringMan::toString(
                                obj->getContinente()->getNumLoc()
                        );
                }
                fprintf( f, "%s\t1", aux.c_str() );

                // Palabra del vocabulario
                fprintf( f, "\t_\t_\t%s\t_\n",
                        obj->getNomVoc().c_str()
                );

                obj = tds->getSigObj();
        }

        fclose( f );
}

// =================================================================== AGEPlugin
AGEPlugin::AGEPlugin(const std::string &a, const std::string &n,
                         const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

AGEPlugin * AGEPlugin::creaAGEPlugin()
{
        AGEPlugin * toret = new(std::nothrow) AGEPlugin(
                                "Baltasar", "age", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para el "
                                "sistema AGE."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin age" );
        }

        return toret;
}

void AGEPlugin::procesar(TDS *tds)
{
        Localidad *loc;
        Objeto * obj;
        std::ostringstream cabDir;
        std::string hora = StringMan::toString( std::time( NULL ) );
        size_t numObjs;

        // Abrir archivo
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".age" );
        FILE *f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        escribirCabecera( f );

        fprintf( f, "<!-- generado por %s@%s, %s/%s\n! %s-->\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        escribeAperturaMarca( f,
                "World worldName=\"" + tds->nombreAventura + "\""
                " moduleName=\"txtMap_ac_" + nomFichSal + "\""
                " date=\"" + hora + "\""
                " type=\"ac\""
                " version=\"0.1\""
                " author=\"" + getNombre() + '@' + nombre + '\"'
        );

        // Meter al jugador
        fprintf( f, "<PlayerGeneration>\n"
                    "<Template id=\"20000000\" name=\"Nuestro Héroe\" "
                    "extends=\"0\" clones=\"0\" type=\"null\" "
                    "properName=\"true\" hp=\"100\" mp=\"0\" maxhp=\"100\" "
                    "maxmp=\"0\" gender=\"true\">\n"
                    "<PropertyList>\n"
                    "<PropertyEntry name=\"state\" value=\"1\" "
                    "timeUnitsLeft=\"1\" />\n"
                    "</PropertyList>\n"
                    "<CurrentRoom id=\"%s\"/>\n"
                    "<LastRoom id=\"%s\"/>\n"
                    "<DescriptionList><Description>"
                    "Nada de particular.</Description></DescriptionList>\n",
                    tds->locComienzo->getId().c_str(),
                    tds->locComienzo->getId().c_str()
        );

        // Relatar objetos que lleva el jugador al principio
        escribeAperturaMarca( f, "Inventory" );
        for(size_t i = 0; i < tds->getListaInventarioObj()->size(); ++i ) {
            getAtributos().carga( AtributoXML( "id", ( ( *tds->getListaInventarioObj() )[ i ] )->getId().c_str() ) );
            escribeAutoMarca( f, "ItemRef" );
        }
        escribeCierreMarca( f, "Inventory" );

        fprintf( f, "<SingularNames><Description>Jugador</Description>"
                    "</SingularNames>\n"
                    "<PluralNames><Description>Jugadores</Description>"
                    "</PluralNames>\n"
                    "<SingularReferenceNames><Name>Jugador</Name>"
                    "</SingularReferenceNames>\n"
                    "<PluralReferenceNames><Name>Jugadores</Name>"
                    "</PluralReferenceNames>\n"
                    "<Parts><Inventory><ItemRef id=\"Cuerpo\"/></Inventory></Parts>\n"
                    "</Template>\n</PlayerGeneration>\n\n"
        );

        // Escribir todos y cada uno de las localidades definidas
        escribeAperturaMarca( f, "Rooms" );

        loc = tds->getPriLoc();
        while( !tds->esLocalidadFinal() )
        {
            getAtributos().carga( AtributoXML( "name", loc->getId() ) );
            escribeAperturaMarca( f, "Room" );

            // Guardar la info de la loc
            escribeAperturaMarca( f, "DescriptionList" );
            guardarCampo( f, "Description", loc->getDesc() );
            if ( !loc->getComentario().empty() ) {
                guardarCampo( f, "Description", loc->getComentario() );
            }
            escribeCierreMarca( f, "DescriptionList" );

            // Relatar objetos que se encuentran en la loc
            escribeAperturaMarca( f, "Inventory" );
            for( obj=loc->getPriObj(); !loc->esObjetoFinal(); obj = loc->getSigObj() ) {
                getAtributos().carga( AtributoXML( "id", obj->getId() ) );
                escribeAutoMarca( f, "ItemRef" );
            }
            escribeCierreMarca( f, "Inventory" );

            // Salidas ?
            escribeAperturaMarca( f, "PathList" );
            for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
            {
                    if ( !(loc->getSalidas())[i].empty() ) {
                            getAtributos().carga( AtributoXML( "destination", (loc->getSalidas())[i] ) );
                            getAtributos().carga( AtributoXML( "direction", Localidad::strDireccion[i] ) );
                            getAtributos().carga( AtributoXML( "standard", "true" ) );

                            escribeAutoMarca( f, "Path" );
                    }
            }

            escribeCierreMarca( f, "PathList" );

            // Music. y grafs.
            if ( !( loc->nombreRecGrafico.empty() ) ) {
                AtributoXML path( "path", loc->nombreRecGrafico );
                getAtributos().carga( path );
                escribeAutoMarca( f, "Image" );
            }

            if ( !( loc->nombreRecMusica.empty() ) ) {
                getAtributos().carga( AtributoXML ( "path", loc->nombreRecMusica ) );
                getAtributos().carga( AtributoXML ( "once", "false" ) );
                getAtributos().carga( AtributoXML ( "loop", "true" ) );
                getAtributos().carga( AtributoXML ( "stopOnExit", "true" ) );

                escribeAutoMarca( f, "Sound" );
            }



            // Objetos static... hay objetos ?
            if ( loc->getNumObjs() > 0 )
            {
                // Cuantos static ?
                obj = loc->getPriObj();
                numObjs = 0;
                for(; !loc->esObjetoFinal(); obj = loc->getSigObj() )
                {
                    if ( obj->esEscenario() ) {
                        ++numObjs;
                    }
                }

                if ( numObjs > 0 )
                {
                    escribeAperturaMarca( f, "ExtraDescriptionList" );

                    obj = loc->getPriObj();
                    for(; !loc->esObjetoFinal(); obj = loc->getSigObj() )
                    {
                        if ( obj->esEscenario() ) {
                            escribeAperturaMarca( f, "ExtraDescription" );
                            guardarCampo( f, "Name", obj->getId() );
                            escribeAperturaMarca( f, "DescriptionList" );
                            guardarCampo( f, "Description", obj->getDesc() );
                            escribeCierreMarca( f, "DescriptionList" );
                            escribeCierreMarca( f, "ExtraDescription" );
                        }
                    }

                    escribeCierreMarca( f, "ExtraDescriptionList" );
                }
            }

            escribeCierreMarca( f, "Room" );

            loc = tds->getSigLoc();
        }

        escribeCierreMarca( f, "Rooms" );

        // Ahora los objetos
        escribeAperturaMarca( f, "Items" );
        for( obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj() ) {
            getAtributos().carga( AtributoXML ( "name", obj->getId() ) );
            getAtributos().carga( AtributoXML ( "volume", "5" ) );
            getAtributos().carga( AtributoXML ( "weight", "1" ) );
            getAtributos().carga( AtributoXML ( "gender", obj->esFemenino() ? "true" : "false" ) );
            if ( obj->esPonible() ) {
                getAtributos().carga( AtributoXML ( "type", "wearable" ) );
            }
            escribeAperturaMarca( f, "Item" );

            escribeAperturaMarca( f, "DescriptionList" );
            guardarCampo( f, "Description", obj->getDesc() );
            escribeCierreMarca( f, "DescriptionList" );

            if ( obj->esPonible() ) {
                escribeAperturaMarca( f, "WearableSpecs" );
                escribeAperturaMarca( f, "WearableLimbs" );
                escribeAperturaMarca( f, "Requirement" );
                guardarCampo( f, "Name", "Cuerpo" );
                escribeCierreMarca( f, "Requirement" );
                escribeCierreMarca( f, "WearableLimbs" );
                escribeCierreMarca( f, "WearableSpecs" );
            }

            escribeCierreMarca( f, "Item" );
        }
        escribeCierreMarca( f, "Items" );

        // Finalizar
        escribeAutoMarca( f, "Mobiles" );
        escribeCierreMarca(f, "World");
}

// =================================================================== KenshiraPlugin
const std::string KenshiraPlugin::Salidas[] = {
        "al_n",
        "al_s",
        "al_e",
        "al_o",
        "arriba",
        "abajo",
        "al_ne",
        "al_so",
        "al_no",
        "al_se",
        "adentro",
        "afuera"
};

KenshiraPlugin::KenshiraPlugin(const std::string &a, const std::string &n,
                                const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

void KenshiraPlugin::procesar(TDS *tds)
{
        Localidad * locIni = tds->locComienzo;
        Localidad * loc;
        Objeto *obj;
        std::string aux;
        std::string objetosInventario;
        std::string hora = StringMan::toString( time( NULL ) );

        // Abrir archivo
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".k" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar la cabecera
        fprintf( f, "// generado por %s@%s, %s/%s\n// %s\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Los includes, clases, el init, y el objeto jugador
        fprintf( f, "\ninclude system/default\n\n\n" );
        fprintf( f, "def init\n\n\tcambiarJugador( jugador )\n\t#jugador.accion( Mirar )\n\nend\n\n\n" );
        fprintf( f,
            "class Habitacion:Localidad\nendclass\nclass Componente:Decorado\nendclass\n"
            "class Cosa\nendclass\n"
        );
        fprintf( f, "\n\nobject jugador:Personaje dentro:%s\n\tnombre MS:jugador", locIni->getIdUnico().c_str() );
        fprintf( f, "\n\tvar DescLarga = \"Tú eres el protagonista.\"\nendobject\n\n" );

        // locs
        fprintf ( f, "\n// ================= LOCALIDADES ===============\n\n" );

        loc = tds->getPriLoc();

        while( !tds->esLocalidadFinal() )
        {
                // Guardar el comentario (si existe)
                if ( !loc->getComentario().empty() ) {
                        fprintf( f, "\n// [%s]\n", loc->getComentario().c_str() );
                }

                // Guardar el encabezado
                fprintf( f, "\nobject %s:Habitacion\n\tnombre %s\n",
                        loc->getIdUnico().c_str(),
                        loc->getId().c_str()
                );

                // Guardar la desc.
                fprintf( f, "\n\tdef DescLarga\n\t\tprintln \"%s\"\n\tend\n\n",
                        ( StringMan::cambiarCadenas(
                            StringMan::cambiarCadenas(
                                loc->getDesc(), '"', "\\\""
                            ), '\n', "^\\\n" )
                        ).c_str()
                );

                // Guardar las direcciones
                for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                {
                        if ( !((loc->getSalidas())[i]).empty() ) {
                            fprintf( f, "\tvar %s=%s\n",
                                     Salidas[ i ].c_str(),
                                     Localidad::cnvtId(
                                                (loc->getSalidas())[i] ).c_str()
                            );
                        }
                }

                fprintf( f, "\nendobject\n" );

                loc = tds->getSigLoc();
        }

        fprintf ( f, "\n// ================= OBJETOS ===============\n\n" );

        obj = tds->getPriObj();
        while( !tds->esObjetoFinal() )
        {
                // Escribir la info del objeto
                fprintf( f, "\nobject %s:%s %s:%s\n\tnombre %s:%s\n\t%s\n\t",
                        obj->getIdUnico().c_str(),
                        ( obj->esEscenario() ? "Componente" : "Cosa" ),
                        esObjetoInventario( tds, obj ) ? ( obj->esPonible() ? "encima" : "dentro" ) : ( obj->esEscenario() ? "componente" : "dentro" ),
                        esObjetoInventario( tds, obj ) ? "jugador" : obj->getContinente()->getIdUnico().c_str(),
                        obj->esFemenino() ? ( obj->esPlural() ? "FP" : "FS" ) : ( obj->esPlural() ? "MP" : "MS" ),
                        StringMan::mins( obj->getNomVoc() ).c_str(),
                        obj->esPonible() ? "flags prenda" : ""

                );

                fprintf( f, "\n\tdef DescLarga\n\t\tprintln \"%s\"\n\tend\n\nendobject\n\n",
                        ( StringMan::cambiarCadenas(
                            StringMan::cambiarCadenas(
                                obj->getDesc(), '"', "\\\""
                            ), '\n', "^\\\n" )
                        ).c_str()
                );

                obj = tds->getSigObj();
        }

        fclose( f );
}

KenshiraPlugin * KenshiraPlugin::creaKenshiraPlugin()
{
        KenshiraPlugin * toret = new(std::nothrow) KenshiraPlugin(
                                "Baltasar", "kenshira", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para el "
                                "sistema Kenshira."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin kenshira" );
        }

        return toret;
}

// =================================================================== I7Plugin
const std::string I7Plugin::Salidas[] = {
        "north",
        "south",
        "east",
        "west",
        "up",
        "down",
        "northeast",
        "southwest",
        "northwest",
        "shoutheast",
        "inside",
        "outside"
};

I7Plugin::I7Plugin(const std::string &a, const std::string &n,
                   const std::string &v, const std::string &h, bool I7Sp)
        : Plugin(a, n, v, h), _esI7Sp( I7Sp )
{
}

void I7Plugin::describirObjeto( TDS * tds, FILE * f, Objeto * obj, bool llevado)
{
    bool esInventario = esObjetoInventario( tds, obj ) ;

    if ( ( !llevado
        && !esInventario )
      || ( llevado
        && esInventario ) )
    {
        // Escribir la info del objeto
        if ( !llevado )
                fprintf( f, "The %s is here. ", obj->getIdUnico().c_str() );
        else    fprintf( f, "The %s is a thing. ", obj->getIdUnico().c_str() );

        // Descripcion principal
        fprintf( f, "The description is \"%s\".\n"
                    "The printed name is \"%s\". "
                    "Understand \"%s\" as %s. ",
            obj->getDesc().c_str(),
            StringMan::mins( obj->getNomVoc() ).c_str(),
            StringMan::mins( obj->getNomVoc() ).c_str(),
            obj->getIdUnico().c_str()
        );

        if ( obj->esEscenario() ) {
            fprintf( f, "It is scenery. " );
        }

        if ( obj->esPonible() ) {
            fprintf( f, "It is wearable. " );
        }

        if ( esI7Sp() ) {
            // Genero
            fprintf( f, "It is %s. ",
                    obj->esFemenino() ? "female" : "male"
            );

            // Num.
            if ( obj->esPlural() ) {
                fprintf( f, "It is plural-named. " );
            }
        }

        fprintf( f, "\n\n" );
    }
}

void I7Plugin::describirLocalidad( FILE * f, Localidad *loc, TDS * tds, Localidad * from, Localidad::Direccion dir)
{
    static int secc = 0;
    std::string dondeEsta;
    Objeto * obj;
    Localidad * locDestino;
    Localidad::Direccion dirOut;


    // La secc.
    ++secc;
    fprintf( f, "\nSection %d - %s\n\n", secc, loc->getId().c_str() );

    // Guardar el comentario (si existe)
    if ( !loc->getComentario().empty() ) {
            fprintf( f, "\n[ %s ]\n", loc->getComentario().c_str() );
    }

    // Guardar el encabezado y la desc.
    fprintf( f, "\nThe %s is a room. The printed name is \"%s\". "
                "The description is \"%s\".\n\n",
            loc->getIdUnico().c_str(),
            loc->getId().c_str(),
            loc->getDesc().c_str()
    );

    // Guardar el de donde viene
    if ( from != NULL ) {
        dondeEsta = loc->getIdUnico()
                        + " is "
                        + Salidas[(size_t) dir ]
                        + " of "
                        + from->getIdUnico()
                        + ".\n\n"
        ;

        fprintf( f, dondeEsta.c_str() );
    }

    // Guardar los objetos
    obj = loc->getPriObj();
    while( !loc->esObjetoFinal() )
    {
        if ( !esObjetoInventario( tds, obj ) ) {
            fprintf( f, "\n\n" );
            describirObjeto( tds, f, obj );
        }

        obj = loc->getSigObj();
    }

    fprintf( f, "\n" );

    // Llamar recursivamente por cada salida
    for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
    {
        dirOut = (Localidad::Direccion) i;
        if ( !((loc->getSalidas())[i]).empty() ) {
            locDestino = tds->buscaLoc( loc->getSalidas()[ i ] );

            if ( locDestino != NULL ) {
                if ( find( locsVisitadas.begin(), locsVisitadas.end(), locDestino )
                          == locsVisitadas.end() )
                {
                        locsVisitadas.push_back( locDestino );
                        describirLocalidad( f, locDestino, tds, loc, dirOut );
                }
            } else    fprintf( f, "\n[ Error buscando salida ]\n" );

        }
    }
}

void I7Plugin::describirObjetosInventario(FILE * f, TDS * tds)
{
    TDS::ListaInventarioObj * l = tds->getListaInventarioObj();
    TDS::ListaInventarioObj::const_iterator it;

    for( it = l->begin(); it != l->end(); ++it) {
        if ( (*it)->esPonible() )
                fprintf( f, "The player wears a %s. ", (*it)->getIdUnico().c_str() );
        else    fprintf( f, "The player carries a %s. ", (*it)->getIdUnico().c_str() );

        describirObjeto( tds, f, *it, true );
    }
}

void I7Plugin::procesar(TDS *tds)
{
        std::time_t ahora;
        std::tm * tiempo = std::localtime( &ahora );
        std::string hora = StringMan::toString( time( NULL ) );
        locsVisitadas.clear();
        locsVisitadas.reserve( tds->getNumLocs() );

        // Abrir la salida
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".i7" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar la cabecera
        fprintf( f, "[\n\t%s@%s, %s/%s\n\t%s\n]\n\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        fprintf( f, "\"%s\" by Maluva.\n\n", tds->nombreAventura.c_str() );

        // Los includes, clases ...
        fprintf( f, "Part 0 - Library Card, Includes\n\n" );
        fprintf( f, "The story genre is \"fantastic\".\n" );
        fprintf( f, "The story headline is \"%s\".\n", tds->nombreAventura.c_str() );
        fprintf( f, "The release number is 1.\nThe story creation year is %d.\n",
                 tiempo->tm_year + 1900
        );
        fprintf( f, "The story description is \"An interactive story\".\n" );

        if ( esI7Sp() )
                fprintf( f, "Include Spanish by Sebastian Arg.\n\n" );
        else    fprintf( f, "\n\n" );

        fprintf( f, "Part 1 - Initializations\n\n" );
        fprintf( f, "[ Playing ]\n" );
        describirObjetosInventario( f, tds );
        fprintf( f, "\nWhen play begins, say \"Playing ...\".\n\n" );

        // locs
        fprintf( f, "Part 2 - World" );
        fprintf( f, "\n\n[ ================= WORLD =============== ]\n\n" );

        locsVisitadas.push_back( tds->locComienzo );
        describirLocalidad( f, tds->locComienzo, tds, NULL, Localidad::LIMBO );

        fclose( f );
}

I7SpPlugin * I7SpPlugin::creaI7SpPlugin()
{
        I7SpPlugin * toret = new(std::nothrow) I7SpPlugin(
                                "Baltasar", "I7sp", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para el "
                                "sistema Inform 7 [ES/spanish]."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin I7sp" );
        }

        return toret;
}

I7GbPlugin * I7GbPlugin::creaI7GbPlugin()
{
        I7GbPlugin * toret = new(std::nothrow) I7GbPlugin(
                                "Baltasar", "I7gb", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como entrada para el "
                                "sistema Inform 7 [EN/english]."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin I7sp" );
        }

        return toret;
}

// ================================================================== BasPlugin
const std::string BasPlugin::strDireccion[] = {
        "N", "S", "E", "W", "U", "D",
        "NE", "SW", "NW", "SE", "ENTER", "EXIT"
};

BasPlugin::BasPlugin(const std::string &a, const std::string &n,
                     const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

void BasPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        Objeto *obj;
        std::string hora = StringMan::toString( std::time( NULL ) );
        int numLinea;

        // Abrir la salida
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".bas" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar la cabecera
        fprintf( f, "10 rem generado por %s@%s, %s/%s\n20 rem %s30 restore:goto 9000\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // locs
        numLinea = NumLineaComienzo;
        fprintf( f, "%d rem *** Story data ***\n", numLinea );
        fprintf( f, "%d rem *** Rooms ***\n", numLinea += 5 );
        fprintf( f, "%d let MaxLocs = %d:dim h$(MaxLocs)"
                    ":rem dim h$(MaxLocs,255):rem spectrum\n",
                        numLinea += 5, tds->getNumLocs()
        );
        fprintf( f, "%d for n = 1 to MaxLocs\n", numLinea += 5 );
        fprintf( f, "%d read h$(n)\n", numLinea += 5 );
        fprintf( f, "%d next n\n", numLinea += 5 );

        loc = tds->getPriLoc();

        size_t i = 0;
        while( !tds->esLocalidadFinal() )
        {
            fprintf( f, "%d data \"%s. %s\"\n", numLinea += 5,
                        loc->getId().c_str(),
                        StringMan::cambiarCadenas(
                            StringMan::cambiarCadenas(
                                loc->getDesc(), '"', "'"
                            ), '\n', " "
                        ).c_str()
            );

            loc->numId = ++i;
            loc = tds->getSigLoc();
        }

        // Guardar las direcciones
        fprintf( f, "%d rem *** Dirs ***\n", numLinea += 5 );
        fprintf( f, "%d let MaxDirs = 12:dim d(MaxLocs,MaxDirs)\n", numLinea += 5 );
        fprintf( f, "%d for n = 1 to MaxLocs\n", numLinea += 5 );
        fprintf( f, "%d for i = 1 to MaxDirs\n", numLinea += 5 );
        fprintf( f, "%d read d(n,i)\n", numLinea += 5 );
        fprintf( f, "%d next i\n", numLinea += 5 );
        fprintf( f, "%d next n\n", numLinea += 5 );

        // REM de ayuda con info sobre direcciones
        fprintf( f, "%d rem ", numLinea += 5 );
        for(size_t i = 0; i < Localidad::NumDirecciones; ++i) {
                fprintf( f, "%s ", strDireccion[ i ].c_str() );
        }
        fprintf( f, "\n" );

        loc = tds->getPriLoc();
        while( !tds->esLocalidadFinal() )
        {
            fprintf( f, "%d data ", numLinea += 5 );

            for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
            {
                const std::string &salida = ( loc->getSalidas() )[i];

                if ( salida.empty() ) {
                    fprintf( f, "0" );
                }
                else {
                    Localidad * locDest = tds->buscaLoc( salida );

                    if ( locDest != NULL )
                            fprintf( f, "%d", locDest->numId );
                    else    fprintf( f, "-1" );
                }

                // Poner una coma ... o no.
                if ( i + 1 < Localidad::NumDirecciones ) {
                    fprintf( f, "," );
                }
            }

            fprintf( f, "\n" );
            loc = tds->getSigLoc();
        }

        fprintf( f, "%d rem *** OBJS ***\n", numLinea += 5 );
        fprintf( f, "%d let MaxObjs = %d:dim o$(MaxObjs):"
                    "rem dim o$(MaxObjs,255):rem spectrum\n",
                                numLinea += 5, tds->getNumObjs()
        );
        fprintf( f, "%d for n = 1 to MaxObjs\n", numLinea += 5 );
        fprintf( f, "%d read o$(n)\n", numLinea += 5 );
        fprintf( f, "%d next n\n", numLinea += 5 );
        fprintf( f, "%d return\n", numLinea += 5 );

        obj = tds->getPriObj();
        while( !tds->esObjetoFinal() )
        {
            fprintf( f, "%d data \"%s\"\n", numLinea += 5,
                        StringMan::cambiarCadenas(
                            StringMan::cambiarCadenas(
                                obj->getDesc(), '"', "'"
                            ), '\n', " "
                        ).c_str()
            );

            obj = tds->getSigObj();
        }

        fprintf( f, "%d rem *** ATRS OBJS ***\n", numLinea += 5 );
        fprintf( f, "%d let MaxAtrs=3:dim o(MaxObjs,MaxAtrs):let AtrNumLoc=1:let AtrStatic=2:let AtrWear=3\n", numLinea += 5 );
        fprintf( f, "%d for n = 1 to MaxObjs\n", numLinea += 5 );
        fprintf( f, "%d for i = 1 to MaxAtrs\n", numLinea += 5 );
        fprintf( f, "%d read o(n,i)\n", numLinea += 5 );
        fprintf( f, "%d next i\n", numLinea += 5 );
        fprintf( f, "%d next n\n", numLinea += 5 );
        fprintf( f, "%d rem numLoc, scenery, wearable\n", numLinea += 5 );

        obj = tds->getPriObj();
        while( !tds->esObjetoFinal() )
        {
            fprintf( f, "%d data %d,%d,%d\n",
                    numLinea += 5,
                    obj->getContinente()->numId,
                    obj->esEscenario() ? 1 : 0,
                    obj->esPonible() ? 1 : 0
            );

            obj = tds->getSigObj();
        }

        // motor juego (bueh ...)
        fprintf( f, "9000 rem *** playing engine draft (sort of...)***\n" );
        fprintf( f, "9010 print \"Loading ...\": gosub %d\n", NumLineaComienzo);
        fprintf( f, "9020 print \"Enter initial of any compass direction. QUIT ends. Only capitals.\"\n" );
        fprintf( f, "9040 print \"Introduzca la inicial de cualquier direccion del compas. FIN termina. Use mays.\"\n" );
        fprintf( f, "9050 let room=%d\n", tds->locComienzo->numId );
        fprintf( f, "9060 print:print h$(room)\n" );
        fprintf( f, "9070 print:input \">\"; i$\n" );
        fprintf( f, "9080 if i$=\"N\" then if d(room,1) > 0 then let room=d(room,1):goto 9060\n" );
        fprintf( f, "9090 if i$=\"S\" then if d(room,2) > 0 then let room=d(room,2):goto 9060\n" );
        fprintf( f, "9100 if i$=\"E\" then if d(room,3) > 0 then let room=d(room,3):goto 9060\n" );
        fprintf( f, "9110 if i$=\"O\" or i$=\"W\" then if d(room,4) > 0 then let room=d(room, 4):goto 9060\n" );
        fprintf( f, "9120 if i$=\"U\" then if d(room,5) > 0 then let room=d(room,5):goto 9060\n" );
        fprintf( f, "9130 if i$=\"D\" then if d(room,6) > 0 then let room=d(room,6):goto 9060\n" );
        fprintf( f, "9140 if i$=\"NE\" then if d(room,7) > 0 then let room=d(room,7):goto 9060\n" );
        fprintf( f, "9150 if i$=\"SW\" or i$=\"SO\" then if d(room,8) > 0 then let room=d(room,8):goto 9060\n" );
        fprintf( f, "9160 if i$=\"NW\" or i$=\"NO\" then if d(room,9) > 0 then let room=d(room,9):goto 9060\n" );
        fprintf( f, "9170 if i$=\"SE\" then if d(room,10) > 0 then let room=d(room,10):goto 9060\n" );
        fprintf( f, "9180 if i$=\"ENTER\" or i$=\"ENTRAR\" then if d(room,11) > 0 then let room=d(room,11):goto 9060\n") ;
        fprintf( f, "9190 if i$=\"EXIT\" or i$=\"SALIR\" then if d(room,12) > 0 then let room=d(room,12):goto 9060\n" );
        fprintf( f, "9200 if i$=\"L\" or i$=\"M\" or i$=\"MIRAR\" or i$=\"LOOK\" then goto 9060\n" );
        fprintf( f, "9210 if i$=\"QUIT\" or i$=\"Q\" or i$=\"SALIR\" or i$=\"FIN\" then stop\n" );
        fprintf( f, "9300 print \"?\": goto 9070\n" );

        fclose( f );
}

BasPlugin * BasPlugin::creaBasPlugin()
{
        BasPlugin * toret = new(std::nothrow) BasPlugin(
                                "Baltasar", "bas", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como parte de un programa "
                                "hecho en Basic."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin basic" );
        }

        return toret;
}

// ================================================================== StdCPlugin
const std::string StdCPlugin::strDireccionEN[] = {
        "N", "S", "E", "W", "U", "D",
        "NE", "SW", "NW", "SE", "ENTER", "EXIT"
};

const std::string StdCPlugin::strDireccionES[] = {
        "N", "S", "E", "O", "ARRIBA", "ABAJO",
        "NE", "SO", "NO", "SE", "ENTRAR", "SALIR"
};

const std::string * StdCPlugin::strDireccion = strDireccionEN;


StdCPlugin::StdCPlugin(const std::string &a, const std::string &n,
                       const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

void StdCPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        Objeto *obj;
        std::string hora = StringMan::toString( time( NULL ) );
        int numLoc = 0;

        // Abrir la salida
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".c" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar la cabecera
        fprintf( f, "/*\n\tgenerado por %s@%s, %s/%s\n\t%s\n*/\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // includes
        fprintf( f, "#include <stdio.h>\n" );
        fprintf( f, "#include <ctype.h>\n" );
        fprintf( f, "#include <string.h>\n" );
        fprintf( f, "\n" );

        if ( getLengua() == ES ) {
            fprintf( f, "#define ES\n" );
            strDireccion = strDireccionES;
        }
        else strDireccion = strDireccionEN;

        // inic
        fprintf( f, "#define MaxTitle 255\n" );
        fprintf( f, "#define MaxExits 12\n" );
        fprintf( f, "#define MaxAtrs 3\n" );
        fprintf( f, "#define AtrPlacedIn 0\n" );
        fprintf( f, "#define AtrWearable 1\n" );
        fprintf( f, "#define AtrScenery 2\n" );
        fprintf( f, "/* Last room is the player */\n" );
        fprintf( f, "#define MaxRooms %d\n", tds->getNumLocs() + 1 );
        fprintf( f, "#define MaxObjects %d\n", tds->getNumObjs() );
        fprintf( f, "#define MaxChars 1024\n" );
        fprintf( f, "#define North 0\n" );
        fprintf( f, "#define South 1\n" );
        fprintf( f, "#define East 2\n" );
        fprintf( f, "#define West 3\n" );
        fprintf( f, "#define Up 4\n" );
        fprintf( f, "#define Down 5\n" );
        fprintf( f, "#define NorthEast 6\n" );
        fprintf( f, "#define SouthWest 7\n" );
        fprintf( f, "#define NorthWest 8\n" );
        fprintf( f, "#define SouthEast 9\n" );
        fprintf( f, "#define Enter 10\n" );
        fprintf( f, "#define Exit 11\n\n" );
        fprintf( f, "#ifndef ES\n" );
        fprintf( f, "const char * Look = \"LOOK\";\n" );
        fprintf( f, "const char * Quit = \"QUIT\";\n" );
        fprintf( f, "const char * YouCanSee = \"You can see:\";\n" );
        fprintf( f, "const char * SeeNothing = \"Nothing special\";\n" );
        fprintf( f, "const char * YouCantDoThat = \"You can't do that\";\n" );
        fprintf( f, "const char * InvalidExit = \"You cnat' go that way\";\n" );
        fprintf( f, "#else\n" );
        fprintf( f, "const char * Look = \"MIRAR\";\n" );
        fprintf( f, "const char * Quit = \"FIN\";\n" );
        fprintf( f, "const char * YouCanSee = \"Puedes ver:\";\n" );
        fprintf( f, "const char * SeeNothing = \"Nada en especial.\";\n" );
        fprintf( f, "const char * YouCantDoThat = \"No puedes hacer eso.\";\n" );
        fprintf( f, "const char * InvalidExit = \"No puedes tomar ese camino.\";\n" );
        fprintf( f, "#endif\n\n" );
        fprintf( f, "\n/* Data structs ---------------------- */" );
        fprintf( f, "\n\ntypedef struct _Room {\n\tchar title[MaxTitle];\n\tchar desc[MaxChars];"
                    "\n\tint exit[MaxExits]\n} Room;\n\n"
        );
        fprintf( f, "typedef struct _Object {\n\tchar voc[MaxTitle];\n\tchar name[MaxTitle];\n\tchar desc[MaxChars];"
                    "\n\tint wearable:1;\n\tint scenery:1;\n\tint placedIn;\n} Object;\n\n"
        );
        fprintf( f, "typedef struct _Story {\n\tchar nombre[MaxTitle];\n\tRoom rooms[MaxRooms];"
                    "\n\tObject objects[MaxObjects]\n} Story;\n\n" );

        fprintf( f, "/* Rooms ---------------------- */\n\n" );

        fprintf( f, "char roomTitles[MaxRooms][MaxTitle] = {\n");
        for(loc = tds->getPriLoc(); !tds->esLocalidadFinal(); loc = tds->getSigLoc()) {
            std::string title = StringMan::cambiarCadenas( loc->getId(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( title, '"', "\\\"" );

            fprintf( f, "\t\"%s\",\n", title.c_str() );

            loc->numId = numLoc++;
        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "char roomDescs[MaxRooms][MaxChars] = {\n");
        for(loc = tds->getPriLoc(); !tds->esLocalidadFinal(); loc = tds->getSigLoc()) {
            std::string desc = StringMan::cambiarCadenas( loc->getDesc(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( desc, '"', "\\\"" );

            fprintf( f, "\t\"%s\",\n", desc.c_str() );

        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "int roomExits[MaxRooms][MaxExits] = {\n");
        loc = tds->getPriLoc();
        while( !tds->esLocalidadFinal() )
        {
            fprintf( f, "\t" );

            for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
            {
                const std::string &salida = ( loc->getSalidas() )[i];

                if ( salida.empty() ) {
                    fprintf( f, "-1" );
                }
                else {
                    Localidad * locDest = tds->buscaLoc( salida );

                    if ( locDest != NULL )
                            fprintf( f, "%d", locDest->numId );
                    else    fprintf( f, "-1" );
                }

                // Poner una coma ... o no.
                if ( i + 1 < Localidad::NumDirecciones ) {
                    fprintf( f, "," );
                }
            }

            if ( tds->esLocalidadFinal() )
                    fprintf( f, "\n" );
            else    fprintf( f, ",\n" );

            loc = tds->getSigLoc();
        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "const char * Compass[] = {\n" );
        for(size_t i = 0; i < Localidad::NumDirecciones; ++i) {
                fprintf( f, "\t\"%s\"", strDireccion[ i ].c_str() );

                if ( i < ( Localidad::NumDirecciones -1 ) ) {
                    fprintf( f, ",\n" );
                }
        }
        fprintf( f, "\n};\n\n" );

        fprintf( f, "/* Objects ---------------------- */\n\n" );

        fprintf( f, "char objectVocs[MaxObjects][MaxTitle] = {\n");
        for(obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj()) {
            fprintf( f, "\t\"%s\"", obj->getNomVoc().c_str() );

            if ( !tds->esObjetoFinal() ) {
                fprintf( f, ",\n" );
            }
        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "char objectNames[MaxObjects][MaxTitle] = {\n");
        for(obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj()) {
            std::string nombre = StringMan::cambiarCadenas( obj->getId(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( nombre, '"', "\\\"" );

            fprintf( f, "\t\"%s\"", nombre.c_str() );

            if ( !tds->esObjetoFinal() ) {
                fprintf( f, ",\n" );
            }
        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "char objectDescs[MaxObjects][MaxChars] = {\n");
        for(obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj()) {
            std::string desc = StringMan::cambiarCadenas( obj->getDesc(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( desc, '"', "\\\"" );

            fprintf( f, "\t\"%s\"", desc.c_str() );

            if ( !tds->esObjetoFinal() ) {
                fprintf( f, ",\n" );
            }
        }

        fprintf( f, "\n};\n\n" );

        fprintf( f, "char objectAtrs[MaxObjects][MaxAtrs] = {\n");
        for(obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj()) {
            fprintf( f, "\t%d, %d, %d,\n", obj->getContinente()->numId,
                                obj->esPonible() ? 1 : 0,
                                obj->esEscenario() ? 1 : 0
            );
        }

        fprintf( f, "\n};\n\n" );


        // Object loader
        fprintf( f, "void objectsLoader(Story * story)\n" );
        fprintf( f, "{\n\tint i = 0;\n\tfor(; i < MaxObjects; ++i) {\n" );
        fprintf( f, "\t\tstrcpy( story->objects[ i ].desc, objectDescs[ i ] );\n" );
        fprintf( f, "\t\tstrcpy( story->objects[ i ].name, objectNames[ i ] );\n" );
        fprintf( f, "\t\tstrcpy( story->objects[ i ].voc, objectVocs[ i ] );\n" );
        fprintf( f, "\t\tstory->objects[ i ].placedIn = objectAtrs[ i ][ AtrPlacedIn ];\n" );
        fprintf( f, "\t\tstory->objects[ i ].wearable = objectAtrs[ i ][ AtrWearable ];\n" );
        fprintf( f, "\t\tstory->objects[ i ].scenery = objectAtrs[ i ][ AtrScenery ];\n" );
        fprintf( f, "\t}\n}\n\n" );

        // Room loader
        fprintf( f, "void roomsLoader(Story * story)\n" );
        fprintf( f, "{\n\tint i = 0;\n\tfor(; i < MaxRooms; ++i) {\n" );
        fprintf( f, "\t\tstrcpy( story->rooms[ i ].title, roomTitles[ i ] );\n" );
        fprintf( f, "\t\tstrcpy( story->rooms[ i ].desc, roomDescs[ i ] );\n" );
        fprintf( f, "\n\t\tint s = 0;\n\t\tfor(; s < MaxExits; ++s) {\n" );
        fprintf( f, "\t\t\tstory->rooms[ i ].exit[ s ] = roomExits[ i ][ s ];\n" );
        fprintf( f, "\t\t}\n" );
        fprintf( f, "\t}\n}\n\n" );

        // toUpper
        fprintf( f, "void toUpper(char * buffer)\n" );
        fprintf( f, "{\n" );
        fprintf( f, "\tchar * c = buffer;\n" );
        fprintf( f, "\tfor(; *c; ++c ) {\n" );
        fprintf( f, "\t\t*c = toupper( *c );\n" );
        fprintf( f, "\t}\n" );
        fprintf( f, "}\n\n" );

        // Is an exit ?
        fprintf( f, "int isExit(Story * story, char * buffer)\n" );
        fprintf( f, "{\n\tint toret = -1;\n\tint i = 0;"
                    "\n\tfor(; i < MaxExits; ++i) {\n"
        );
        fprintf( f, "\t\tif ( !strcmp( Compass[ i ], buffer ) ) {\n" );
        fprintf( f, "\t\t\ttoret = i;\n\t\t\tbreak;\n" );
        fprintf( f, "\t\t}\n" );
        fprintf( f, "\t}\n\n\treturn toret;\n}\n\n" );

        // describe
        fprintf( f, "void describe(Story * story, int room)\n" );
        fprintf( f, "{\n\tint i;\n\tint numObjs = 0;"
                    "\n\n\tprintf( \"\\n%%s\\n\", story->rooms[ room ].title );"
        );
        fprintf( f, "\n\tprintf( \"%%s\\n\", story->rooms[ room ].desc );" );
        fprintf( f, "\n\tprintf( \"%%s\\n\", YouCanSee );" );
        fprintf( f, "\n\tfor(i = 0; i < MaxObjects; ++i) {" );
        fprintf( f, "\n\t\tif ( story->objects[ i ].placedIn == room ) {" );
        fprintf( f, "\n\t\t\t++numObjs;" );
        fprintf( f, "\n\t\t\tprintf( \"%%s\\n\", story->objects[ i ].name );" );
        fprintf( f, "\n\t\t}" );
        fprintf( f, "\n\t}" );
        fprintf( f, "\n\n\tif ( numObjs == 0 ) {" );
        fprintf( f, "\n\t\tprintf( \"%%s\\n\", SeeNothing );" );
        fprintf( f, "\n\t}" );
        fprintf( f, "\n}\n\n" );

        // getInput
        fprintf( f, "void getInput(char * buffer)\n" );
        fprintf( f, "{\n\tprintf( \"\\n> \" ); gets( buffer ); toUpper( buffer );" );
        fprintf( f, "\n}\n\n" );

        // moveTo
        fprintf( f, "int moveTo(Story * story, int dir, int * room)\n" );
        fprintf( f, "{\n\tint newRoom = story->rooms[ *room ].exit[ dir ];" );
        fprintf( f, "\n\tif ( newRoom >= 0 )\n\t\treturn ( *room = newRoom );"
                    "\n\telse\treturn 0;"
        );
        fprintf( f, "\n}\n\n" );


        // main
        fprintf( f, "int main(void)\n" );
        fprintf( f, "{\n\tStory story;\n\tchar buffer[MaxChars];\n\tint room = %d;\n\n"
                    "\t#ifndef ES\n\tprintf( \"Loading ...\\n\" );\n"
                    "\tprintf( \"Enter compass directions initials to move\\n\" );\n"
                    "\t#else\n"
                    "\tprintf( \"Cargando ...\\n\" );\n"
                    "\tprintf( \"Introduzca las iniciales de las direcciones para moverse\\n\" );\n"
                    "\t#endif\n\n"
                    "\tobjectsLoader( &story );\n\troomsLoader( &story );\n\n",
                    tds->locComienzo->numId
        );
        fprintf( f, "\n\t/* Main loop */\n" );
        fprintf( f, "\n\tdescribe( &story, room );\n" );
        fprintf( f, "\tgetInput( buffer );\n" );
        fprintf( f, "\twhile( strcmp( buffer, Quit ) ) {\n" );
        fprintf( f, "\t\tint dir = isExit( &story, buffer );\n\n" );
        fprintf( f, "\t\tif ( dir > -1 ) {\n" );
        fprintf( f, "\t\t\tif ( moveTo( &story, dir, &room ) )\n"
                    "\t\t\t\tdescribe( &story, room );\n" );
        fprintf( f, "\t\t\telse\tprintf( \"%%s\\n\", InvalidExit );\n" );
        fprintf( f, "\t\t}\n\t\telse\n" );
        fprintf( f, "\t\tif ( !strcmp( buffer, Look ) ) {\n" );
        fprintf( f, "\t\t\tdescribe( &story, room );\n" );
        fprintf( f, "\t\t}\n" );
        fprintf( f, "\t\telse {\n" );
        fprintf( f, "\t\t\tprintf( \"%%s\\n\", YouCantDoThat );\n" );
        fprintf( f, "\t\t}\n" );
        fprintf( f, "\n\t\tgetInput( buffer );\n\n" );
        fprintf( f, "\t}\n\n\treturn 0;\n}\n\n" );


        fclose( f );
}

StdCPlugin * StdCPlugin::creaStdCPlugin()
{
        StdCPlugin * toret = new(std::nothrow) StdCPlugin (
                                "Baltasar", "stdC", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como parte de un programa "
                                "hecho en std. C."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin stdC" );
        }

        return toret;
}

// ================================================================== FiJsPlugin
FiJsPlugin::FiJsPlugin(const std::string &a, const std::string &n,
                       const std::string &v, const std::string &h)
        : Plugin(a, n, v, h)
{
}

void FiJsPlugin::procesar(TDS *tds)
{
        Localidad * loc;
        Objeto *obj;
        int numLoc = 0;
        time_t currentTime = time( NULL );
        std::string hora = StringMan::toString( currentTime );
        std::tm * fecha = std::localtime( &currentTime );

        // Abrir la salida
        StringMan::ponerExtensionAdecuadaNombreArchivo( nomFichSal, ".js" );
        FILE * f = fopen( nomFichSal.c_str(), "wt" );

        if ( f == NULL ) {
                throw ErrorES( "imposible abrir " + nomFichSal );
        }

        // Guardar la cabecera
        fprintf( f, "/*\n\tgenerado por %s@%s, %s/%s\n\t%s\n*/\n\n",
                    getNombre().c_str(),
                    nombre.c_str(),
                    getVersion().c_str(),
                    version.c_str(),
                    hora.c_str()
        );

        // Info de la aventura
        // Titulo & intro
        fprintf( f, "ctrl.ponTitulo( \"%s\" );\n", tds->nombreAventura.c_str() );
        fprintf( f, "ctrl.ponIntro( \"<p>Empieza la aventura...</p>\" );\n" );
        fprintf( f, "// ctrl.ponImg( \"res/portada.jpg\" );\n" );
        fprintf( f, "ctrl.ponAutor( \"txtmap@caad.es\" );\n" );
        fprintf( f, "ctrl.ponVersion( \"%04d%02d%02d\" );\n",
                fecha->tm_year + 1900, fecha->tm_mon + 1, fecha->tm_mday );

        // Localidades
        fprintf( f, "\n// *** Locs --\n" );

        for(loc = tds->getPriLoc(); !tds->esLocalidadFinal(); loc = tds->getSigLoc()) {
            std::string title = StringMan::cambiarCadenas( loc->getId(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( title, '"', "\\\"" );

            std::string desc = StringMan::cambiarCadenas( loc->getDesc(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( desc, '"', "\\\"" );

            fprintf( f, "\nvar %s = ctrl.lugares.creaLoc(\n", loc->getIdUnico().c_str() );
            fprintf( f, "\t\"%s\",\n", title.c_str() );
            fprintf( f, "\t[ \"%s\" ],\n", StringMan::mins( loc->getId() ).c_str() );
            fprintf( f, "\t\"%s\"\n);\n", desc.c_str() );

            if ( !( loc->nombreRecGrafico.empty()) ) {
                fprintf( f, "%s.pic = \"%s\";\n",
                        loc->getIdUnico().c_str(),
                        loc->nombreRecGrafico.c_str() );
            }

            if ( !( loc->nombreRecMusica.empty()) ) {
                fprintf( f, "%s.audio.src = \"%s\";\n",
                        loc->getIdUnico().c_str(),
                        loc->nombreRecMusica.c_str() );
            }

            loc->numId = numLoc++;
        }

        fprintf( f, "\n// *** Compas --\n\n" );
        loc = tds->getPriLoc();
        while( !tds->esLocalidadFinal() )
        {
            fprintf( f, "\n// -- %s\n", loc->getIdUnico().c_str() );

            for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
            {
                const std::string &salida = ( loc->getSalidas() )[i];

                if ( !salida.empty() ) {
                    Localidad * locDest = tds->buscaLoc( salida );

                    if ( locDest != NULL ) {
                            fprintf( f, "%s.ponSalida( \"%s\", %s );\n",
                                    loc->getIdUnico().c_str(),
                                    StringMan::mins( Localidad::strDireccion[ i ] ).c_str(),
                                    locDest->getIdUnico().c_str() );
                    }
                }
            }

            loc = tds->getSigLoc();
        }

        fprintf( f, "\n\n// *** Objs --\n" );
        for(obj = tds->getPriObj(); !tds->esObjetoFinal(); obj = tds->getSigObj()) {
            std::string desc = StringMan::cambiarCadenas( obj->getDesc(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( desc, '"', "\\\"" );

            std::string nombre = StringMan::cambiarCadenas( obj->getId(), '\n', "\\n" );
            StringMan::cambiarCadenasCnvt( nombre, '"', "\\\"" );

            std::string voc = StringMan::mins( obj->getNomVoc() );
            fprintf( f, "\nvar %s = ctrl.creaObj(\n", obj->getIdUnico().c_str() );
            fprintf( f, "\t\"%s\",\n", nombre.c_str() );
            fprintf( f, "\t[ \"%s\" ],\n", voc.c_str() );
            fprintf( f, "\t\"%s\",\n", desc.c_str() );
            fprintf( f, "\t%s,\n", obj->getContinente()->getIdUnico().c_str() );

            if ( obj->esEscenario() ) {
                fprintf( f, "\tEnt.Escenario\n" );
            } else {
                fprintf( f, "\tEnt.Portable\n" );
            }

            fprintf( f, ");\n" );

            if ( obj->esPonible() ) {
                fprintf( f, "%s.ponPrenda();\n", obj->getIdUnico().c_str() );
            }
        }

        fprintf( f, "\n\nctrl.lugares.ponInicio( %s );\n\n",
                tds->getPriLoc()->getIdUnico().c_str()
        );

        fclose( f );
}

FiJsPlugin * FiJsPlugin::creaFiJsPlugin()
{
        FiJsPlugin * toret = new(std::nothrow) FiJsPlugin (
                                "Baltasar", "fi.js", "v0.1",
                                "Este plugin genera un archivo preparado "
                                "para ser utilizado como parte de un programa "
                                "hecho con HTML y fi.js."
        );

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria, creando plugin fi.js" );
        }

        return toret;
}
