// txtmap.cpp
/*
  	Genera un fichero XML a partir del texto
	de una aventura en juego conversacional natural
*/

#include "txtmap.h"
#include "plugin.h"
#include "stringman.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <climits>
#include <cstdlib>

// Comentar esta macro para integrar txtMap en otro programa,
// o bien para sustituir la interfaz de usuario proporcionada por otra
#define __TEXT__UI

const std::string nombre     = "txtMap";
const std::string version    = " v0.6 20140612";

const std::string opQuiet    = "QUIET";
const std::string opLanguage = "LANG=";
const std::string opMaxChars = "MAXCHARS=";
const std::string opClean    = "CLEAN";
const std::string opMotr     = "MOTR";
const std::string opAyudaPl  = "PL?";
const std::string opExePl    = "PL=";
const std::string opOutput   = "O";
const std::string opMkdir    = "MKDIR";
const std::string coment     = "!";
const std::string extlog     = ".log";
const std::string extXML     = ".txm";

const char CHR_PROMPT   = '>';
bool modoQuiet          = false;

// --------------------------------------------------------- Excepciones
std::string Error::toString() const {
        std::string toret = "ERROR " + std::string( what() );

        if ( !( getDetails().empty() ) ) {
                toret += ' ';
                toret += '(';
                toret += getDetails();
                toret += ')';
        }

        return toret;
}

// ---------------------------------------------------------------- fich
const bool Fich::LECTURA   = true;
const bool Fich::ESCRITURA = false;

std::string FichSalida::toString(int x) {
        std::ostringstream o;

       	o << x;

        return o.str();
}

void FichSalida::ponLinea(const std::string &l)
{
	if ( estaAbierto() )
	{
		fputs( l.c_str(), f );
		fflush( f );
	}
}

// ------------------------------------------------------------ FichEntrada
const std::string &FichEntrada::leeLinea(void) {
      int ch;
      buffer.clear();

      if ( estaAbierto() )
      {
		do {
			if ((ch = fgetc(f)) != EOF) {
				buffer.append( 1, (char) ch );
			}
		} while(!feof(f)
		      && ch != '\n');

		ch = fgetc(f);

		if (ch != '\r') {
			ungetc(ch, f);
		}
      }

      return buffer;
}

// ------------------------------------------------------------ Scanner
const std::string Scanner::DELIMITADORES = " \n\t\r";


Scanner::Scanner(const std::string &nf) : nombre(nf), linea(0), blank(0) {
  	f = new(std::nothrow) FichEntrada( nf );

	if ( f == NULL ) {
		throw ErrorInterno( "Sin memoria creando el scanner" );
	}

	if( !f->estaAbierto() ) {
		throw ErrorES( "Error abriendo " + nf );
	}
}

Scanner::~Scanner() {
  	delete f;
}

std::string Scanner::getNomFich(const std::string &nom)
/*
	Devuelve el nombre de fichero sin ext., pero manteniendo el PATH
*/
{
    return StringMan::extraerArchivoSinExtDeNombreArchivo( nom );
}

void Scanner::pasaEsp(const std::string &lin, size_t &pos, const std::string &delim) {

  	while ( pos < lin.length()
         && delim.find(lin[pos]) != std::string::npos )
	{
    		++pos;
	}
}

std::string Scanner::getToken(const std::string &lin, size_t &pos, const std::string &delim)
{
  	std::string tok = "";

  	while ( pos < lin.length()
	     && delim.find(lin[pos]) == std::string::npos )
        {
    		tok = tok + lin[pos];
    		++pos;
  	}

  	return tok;
}

std::string Scanner::getLiteral(const std::string &lin, size_t &pos, char limitador)
/*
        Devuelve un literal, como "hola, mundo", si limitador=='"'
        Si no hay literal, devuelve la cadena nula
*/
{
        std::string toret;

        // Buscar las primeras comillas
        pasaEsp( lin, pos );
        if ( lin[pos] == limitador )
        {
            ++pos;

            // Leerlo todo hasta encontrar un delimitador
            while( lin[ pos ] != limitador
                && pos < lin.length() )
            {
                  toret += lin[pos];
                  ++pos;
            }

            // Pasar el ltimo delimitador
            if (lin[pos] == limitador) {
                ++pos;
            }
            else throw ErrorSintaxis("se esperaban comillas");
        }

        return toret;
}

const std::string &Scanner::leeLinea() {
  	size_t primerCaracter;
    blank = 0;

  	do {
        // Actualizar lin.
        buf = f->leeLinea();
        ++linea;

        // encontrar el primer caracter que no es un espacio
        primerCaracter = 0;
        pasaEsp( buf, primerCaracter );

        // lin. en blanco ?
        if ( primerCaracter > ( buf.length() - 1 )
          || buf[primerCaracter] == '\n'
          || buf[primerCaracter] == '\r'
          || buf.empty() )
        {
            buf = "";
            ++blank;
        }
        else {
              // eliminar lins. que empiezan por comentario
              if (buf.compare(primerCaracter, coment.length(), coment) == 0)
              {
                    buf  = "";
                    ++blank;     // Con esto no se modifican los num. de lin.
                                 // del fichero de salida, excepto las
                                 // de los includes (ficheros insertados)
              }
              else {
                    // lin. normal: eliminar retorno de carro
                    size_t pos = buf.find('\n');

                    // Eliminar 0D, e incluso 0D0A
                    if (pos != std::string::npos) {
                          buf.erase(pos);
                    }

                    // Eliminar 0A, si existe, incluso 0A0D (?)
                    pos = buf.find('\r'); // slo por si acaso
                    if (pos != std::string::npos) {
                          buf.erase(pos);
                    }
              }
          }
  	} while(buf.empty()
        && !finEntrada());

  	return buf;
}

// --------------------------------------------------------- Parser

Parser::Parser() : scanSCE(NULL), fout(NULL), log(NULL) {};

Parser::Parser(Scanner *sc, bool cl)
		: scanSCE(sc), fout(NULL), log(NULL), modoLimpio(cl)
{

        // El nombre del log se crea simplemente incorporando la ext de log
        // con esto el fichero sería fich.ext.log
        nomFich = scanSCE->devNombreEntrada() + extlog;

        if  ( !enModoLimpio() ) {
        	log = new(std::nothrow) FichSalida( nomFich );

            // Podemos permitirnos que el log falle
            ponLogStr( nombre + version );
            ponLogStr( "entrada: " + scanSCE->devNombreEntrada() );
            ponLogStr( "========================" );
        }
};

Parser::~Parser() {
        ponLogStr("Fin del proceso de " + scanSCE->devNombreEntrada()+ " ...");
  	delete fout;
  	delete log;
}

void Parser::ponLogStr(const std::string &txt) {
	std::ostringstream buffer;

        if (   log != NULL
	 && !( enModoLimpio() ) )
        {
		buffer << std::setw( 7 ) << scanSCE->devNumLinea() << ' '
		       << txt     << '\n';

  	        log->ponLinea(buffer.str());
        }
}

void Parser::escribeSalidaParcial(const std::string &x)
{
	if (fout != NULL) {
		fout->ponLinea(x);
	}
}

void Parser::escribeSalida(const std::string &x)
{
        if (fout != NULL) {
  	        fout->ponLinea(x + '\n');
	}
}

// ================================================================= ParserAvNat
const std::string ParserAvNat::strDeterminantes =
                        " el la los las"
                        " estas estos esas esos"
                        " aquellas aquellos"
                        " mi mis tu tus su sus"
                        " nuestras nuestros vuestras vuestros"
                        " un una uno a"
                        // ------------ inglés --------------
                        " the this these those"
                        " my your his her our"
                        " at a "
;

ParserAvNat::ParserAvNat(Scanner *sc, bool cl)
        : Parser(sc, cl)
{
        cambiaEstado( TOPLEVEL );
}

void ParserAvNat::cambiaEstado(Estado e)
{
        estado = e;
}

void ParserAvNat::ignoraDeterminantes(const std::string &lin, size_t &pos)
/**
        Busca todos los determinantes a partir de la cadena actual, de manera
        que devuelve un pos actualizado.
*/
{
        size_t nuevaPos = pos;
        std::string token;

        if ( pos < lin.size() - 1 ) {
          do {
                Scanner::pasaEsp( lin, nuevaPos );
                token = Scanner::getToken( lin, nuevaPos );

                if ( !token.empty() ) {
                        token = ' ' + StringMan::minsCnvt( token ) + ' ';
                        if ( strDeterminantes.find( token ) != std::string::npos) {
                                pos = nuevaPos;
                        }
                }
          } while( pos == nuevaPos );
        }

        return;
}

std::string ParserAvNat::buscaSigLineaCmd(bool capturar)
/**
        Busca la siguiente lin. de la entrada que contiene un comando.
*/
{
        std::string lincnvt;
        noCargar = true;
        std::string toret;

        // Buscar siguiente lin. importante (tiene '>') o EOF
        do {
                // Leer la lin.
                linact = scanSCE->leeLinea();
                lincnvt = StringMan::trim( linact );

                // Si es un comando de control, salir al instante
                if ( lincnvt[ 0 ] == '.' ) {
                        break;
                }

                // Capturar la entrada por si es una desc
                if ( capturar
                  && lincnvt[0] != CHR_PROMPT )
                {
                        toret += linact;
                }
        } while( lincnvt[0] != CHR_PROMPT
              && !( scanSCE->finEntrada() ) );

        return toret;
}

bool ParserAvNat::cambiaEstadoPorObjeto()
{
        size_t pos = 1;
        std::string linea = StringMan::trim( linact );
        StringMan::maysCnvt( linea );
        Scanner::pasaEsp( linea, pos );
        std::string tok = Scanner::getToken( linea, pos );
        tok = ' ' + tok + ' ';
        bool toret = true;

        if ( tok.empty() ) {
                cambiaEstado( NADA );
        }
        else
        if ( Objeto::cmdCOGER.find( tok ) != std::string::npos ) {
                ponLogStr( "tomando objeto" );
                cambiaEstado( OBJ );
        }
        else
        if ( Objeto::cmdPONER.find( tok ) != std::string::npos )
        {
                ponLogStr( "tomando objeto como 'ponible'" );
                cambiaEstado( OBJPONER );
        }
        else
        if ( Objeto::cmdEX.find( tok ) != std::string::npos ) {
                ponLogStr( "tomando desc. objeto" );
                cambiaEstado( OBJDESC );
        }
        else
        if ( Objeto::cmdINVENTARIO.find( tok ) != std::string::npos ) {
                ponLogStr( "tomando inventario" );
                cambiaEstado( INVENTARIO );
        }
        else toret = false;

        return toret;
}

bool ParserAvNat::cambiaEstado(const std::string &linact)
{
        bool toret = true;
        size_t pos = 0;
        std::string linea = StringMan::trim( linact );
        StringMan::maysCnvt( linea );
        Scanner::pasaEsp( linea, pos );
        std::string tok = Scanner::getToken( linea, pos );
        tok = ' ' + tok + ' ';

        // Controlar los estados del AF
        if ( getEstado() == TOPLEVEL )
        {
                cambiaEstado( TIT );
                ponLogStr( "nueva localidad '" + linact + '\'' );
        }
        else
        if ( getEstado() == MOV
          || getEstado() == NADA )
        {
                cambiaEstado( MOV );

                if ( Localidad::cmdFinLoc.find( tok ) != std::string::npos ) {
                        ponLogStr( "FINLOC encontrado" );
                        cambiaEstado( TOPLEVEL );
                }
                else
                if ( linea[0] != CHR_PROMPT ) {
                        cambiaEstado( TIT );
                        ponLogStr( "nueva localidad '" + linact + '\'' );
                }
                else {
                        cambiaEstadoPorObjeto();
                }
        }
        else
        if ( getEstado() == DESC
          || getEstado() == TIT )
        {
                if ( Localidad::cmdGrf.find( tok ) != std::string::npos ) {
                        cambiaEstado( GRF );
                        ponLogStr( "tomando grf. de localidad" );
                }
                else
                if ( Localidad::cmdMsc.find( tok ) != std::string::npos ) {
                        cambiaEstado( MSC );
                        ponLogStr( "tomando msc. de localidad" );
                }
                else
                if ( Localidad::cmdFinLoc.find( tok ) != std::string::npos ) {
                        ponLogStr( "FINLOC encontrado" );
                        cambiaEstado( TOPLEVEL );
                }
                else
                if ( getEstado() == DESC
                 &&  linea[0]    == CHR_PROMPT )
                {
                        // Determinar si es movimiento, ex o coger
                        if ( !cambiaEstadoPorObjeto() ) {
                                cambiaEstado( MOV );
                                ponLogStr( "preparando primer movimiento" );
                        }
                }
                else {
                        cambiaEstado( DESC );
                        ponLogStr( "tomando desc." );
                }
        }
        else
        if ( getEstado() == MOV )
        {
                if ( linea[0] != CHR_PROMPT ) {
                        throw ErrorSintaxis( "se esperaba movimiento" );
                }

                cambiaEstado( MOV );
                ponLogStr( "tomando movimiento" );
        }
    	else toret = false;

  	return toret;
}

void ParserAvNat::crearNuevaLoc(Localidad *&loc, Localidad::Direccion &ultimaDir)
{
        std::string locAnt;

        // ¿Hay loc anterior? ... si hay, enlazarla
        if ( loc != NULL ) {
                locAnt = loc->getId();
                if ( ultimaDir != Localidad::LIMBO ) {
                        loc->ponSalida( ultimaDir, linact );

                        ponLogStr( '\''+ locAnt
                                + "' por "
                                + Localidad::cnvtDireccionAStr( ultimaDir )
                                + " a '" + linact + '\''
                        );
                }
        }

        // Crear la nueva localidad
        loc = new(std::nothrow) Localidad( linact );

        if ( loc == NULL ) {
                throw ErrorInterno( "sin memoria para '" + linact + '\'' );
        }

        // Guardarla en TDS
        tds.insertaLoc( loc );

        // Hacer el doble enlace por defecto
        if ( ultimaDir != Localidad::LIMBO ) {
                Localidad::Direccion dir = Localidad::cnvtDirInversa( ultimaDir );
                ponLogStr( '\'' + linact
                  + "' por [inversa]  "
                  + Localidad::cnvtDireccionAStr( dir )
                  + " a '" + locAnt + '\''
                );

                loc->ponSalida( dir, locAnt );
                ultimaDir = Localidad::LIMBO;
        }
}

void ParserAvNat::procesaMovimiento(
        Localidad *loc,
        Localidad::Direccion &ultimaDir)
{
        std::string idLocalidad;
        std::string lin;

        if ( loc != NULL ) {
            ultimaDir = Localidad::cogeDir( linact, idLocalidad );

            if ( ultimaDir != Localidad::LIMBO )
            {
                if ( !idLocalidad.empty() ) {
                    loc->ponSalida( ultimaDir, idLocalidad );

                    ponLogStr( loc->getId()
                     + " por "
                     + Localidad::cnvtDireccionAStr( ultimaDir )
                     + " a " + idLocalidad );

                    ultimaDir = Localidad::LIMBO;
                }
            }
            else {
                // No es un movimiento, sino otra cosa
                // Pasar hasta encontrar otro posible mov
                buscaSigLineaCmd();
            }
        } else ultimaDir = Localidad::LIMBO;
}

void ParserAvNat::procesaObjeto(Localidad *loc)
{
        std::string nombreObj;
        size_t pos = 1;

        ponLogStr( "Creando/modificando objeto (coger)." );

        // Tomar el nombre del objeto
        // Pasa comando 'coger' y determinantes
        Scanner::pasaEsp( linact, pos );
        Scanner::getToken( linact, pos );
        ignoraDeterminantes( linact, pos );

        // Tomar nombre de objeto
        Scanner::pasaEsp( linact, pos );
        nombreObj = linact.substr( pos, linact.length() );

        Objeto * obj = tds.buscaObjeto( nombreObj );
        if ( obj != NULL ) {
                ponLogStr( "Objeto ya existente: " + nombreObj );
        }
        else {
                obj = tds.insertaObjeto( nombreObj, loc->getId() );
                ponLogStr( "Objeto " + nombreObj );
        }

        obj->ponTransportable();

        buscaSigLineaCmd();
}

void ParserAvNat::procesaObjetoPonible(Localidad *loc)
{
        std::string nombreObj;
        size_t pos = 1;

        ponLogStr( "Creando/modificando objeto (poner)." );

        // Tomar el nombre del objeto
        // Pasa comando 'poner' e ignorar determinantes
        Scanner::pasaEsp( linact, pos );
        Scanner::getToken( linact, pos );
        ignoraDeterminantes( linact, pos );

        // Tomar nombre de objeto
        Scanner::pasaEsp( linact, pos );
        nombreObj = linact.substr( pos, linact.length() );

        Objeto * obj = tds.buscaObjeto( nombreObj );
        if ( obj != NULL ) {
                ponLogStr( "Objeto ya existente: " + nombreObj );
        }
        else {
                obj = tds.insertaObjeto( nombreObj, loc->getId() );
                ponLogStr( "Objeto " + nombreObj );
        }

        obj->ponPonible();

        buscaSigLineaCmd();
}

void ParserAvNat::procesaInventario(Localidad *loc)
{
        std::string nombresObjs;
        std::string nombreObj;
        size_t posAnt = 0;

        ponLogStr( "Añadiendo objetos al inventario." );

        nombresObjs = buscaSigLineaCmd( CapturaTexto );

        // Buscar los objetos en la cadena
        size_t pos = nombresObjs.find( ',', posAnt );

        while( pos != std::string::npos ) {
            nombreObj = nombresObjs.substr( posAnt, pos - posAnt );
            StringMan::trimCnvt( nombreObj );
            tds.getListaInventarioStr()->push_back( nombreObj );
            posAnt = ++pos;

            pos = nombresObjs.find( ',', pos );
        }

        // Recoger el objeto restante
        if ( nombresObjs[ posAnt ] == ',' ) {
                ++posAnt;
        }
        nombreObj = nombresObjs.substr( posAnt, nombresObjs.size() );
        StringMan::trimCnvt( nombreObj );
        tds.getListaInventarioStr()->push_back( nombreObj );
}

void ParserAvNat::procesaObjetoEscenario(Localidad *loc)
{
        std::string nombreObj;
        std::string descObj;
        size_t pos = 1;

        ponLogStr( "Aceptando desc. de objeto "
                     + nombreObj
        );

        // Tomar el nombre del objeto
        // Pasa comando 'ex' e ignorar determinantes
        Scanner::pasaEsp( linact, pos );
        Scanner::getToken( linact, pos );
        ignoraDeterminantes( linact, pos );

        // Tomar nombre de objeto
        Scanner::pasaEsp( linact, pos );
        nombreObj = linact.substr( pos, linact.length() );
        ponLogStr( "Objeto " + nombreObj );

        // Tomar las sigs líneas (descripción)
        descObj = buscaSigLineaCmd( CapturaTexto );

        // Si no existe, crearlo
        Objeto * obj = tds.buscaObjeto( nombreObj );
        if ( obj == NULL ) {
                obj = tds.insertaObjeto( nombreObj, loc->getId() );
                obj->ponEstatico();
        }

        // Darle la descripción
        if ( obj->getDesc().empty() )
                obj->ponDesc( descObj );
        else throw ErrorIdDuplicado( '\''
                                        + obj->getId()
                                        + "' ya tiene una desc. asignada"
        );
}

void ParserAvNat::procEntrada(void) throw(Error)
{
        Localidad::Direccion ultimaDir = Localidad::LIMBO;
        Localidad *loc = NULL;

        noCargar = false;

        // Es correcto el fich de entrada ?
        if (    ( scanSCE == NULL )
          || ( !( scanSCE->preparado() ) ) )
        {
                  throw ErrorES( "entrada inexistente" );
        }

        // Tomar primera línea de la entrada con info
        linact = scanSCE->leeLinea();

        // Cargar localidades y objetos hasta final de fichero
        while ( !( linact.empty() ) ) {
              // ¿Cambiamos de estado?
              if ( cambiaEstado( linact ) )
              {
                if ( getEstado() == TIT )
                {
                        crearNuevaLoc( loc, ultimaDir );
                }
                else
                if ( getEstado() == DESC )
                {
                        loc->ponDesc( loc->getDesc() + '\n' + linact );
                }
                else
                if ( getEstado() == GRF )
                {
                        size_t pos = 0;

                        Scanner::pasaEsp( linact, pos );
                        Scanner::getToken( linact, pos );
                        Scanner::pasaEsp( linact, pos );
                        cambiaEstado( DESC );

                        loc->nombreRecGrafico =
                                Scanner::getLiteral( linact, pos, '"' )
                        ;
                }
                else
                if ( getEstado() == MSC )
                {
                        size_t pos = 0;

                        Scanner::pasaEsp( linact, pos );
                        Scanner::getToken( linact, pos );
                        Scanner::pasaEsp( linact, pos );
                        cambiaEstado( DESC );

                        loc->nombreRecMusica =
                                Scanner::getLiteral( linact, pos, '"' )
                        ;
                }
                else
                if ( getEstado() == MOV )
                {
                        procesaMovimiento( loc, ultimaDir );
                }
                else
                if ( getEstado() == OBJ )
                {
                        procesaObjeto( loc );
                        cambiaEstado( MOV );
                }
                else
                if ( getEstado() == OBJDESC )
                {
                        procesaObjetoEscenario( loc );
                        cambiaEstado( MOV );
                }
                else
                if ( getEstado() == OBJPONER )
                {
                        procesaObjetoPonible( loc );
                        cambiaEstado( MOV );
                }
                else
                if ( getEstado() == INVENTARIO )
                {
                        procesaInventario( loc );
                        cambiaEstado( MOV );
                }
              }

              // Tomar siguiente línea
              muestraProceso( scanSCE, this );

              if ( !noCargar )
                        linact = scanSCE->leeLinea();
              else      noCargar = false;
        }
}

// ======================================================================== Item
size_t Item::numItems = 0;

Item::Item(const std::string &i, const std::string &d) : desc(d), id(i)
{
        idUnico.clear();
        numItem = ++numItems;
        StringMan::trimCnvt( id );
        chkId( id );
}

Item::~Item()
{
}

void Item::chkId(const std::string &id) throw (ErrorSintaxis)
{
        // Es nulo ?
        if ( StringMan::trim( id ).empty() ) {
                throw ErrorSintaxis( "id nulo" );
        }

        // Pasa de 32 caracteres ? (los parsers no suelen soportar)
        // Es necesario contar con los prefijos de grfcs ... etc.
        if ( id.length() > MaxTamId ) {
                throw ErrorSintaxis( "id excede longitud permitida" );
        }

        // Primer caracter
        char ch = StringMan::minsCnvtCh( id[0], StringMan::EliminaAcentos );
        if (  ch != '_'
          && !isalpha( ch ) )
        {
                throw ErrorSintaxis( "id ilegal: '" + id + '\'' );
        }

        // Todos los caracteres
        for(size_t i = 0; i < id.length(); ++i)
        {
                ch = StringMan::minsCnvtCh( id[i], StringMan::EliminaAcentos );

                if ( ch != '_'
                  && ch != '-'
                  && ch != ':'
                  && ch != ' '
                  && !isalpha( ch )
                  && !isdigit( ch ) )
                {
                        throw ErrorSintaxis( "id no permitido: " + id );
                }
        }

        return;
}

std::string Item::cnvtId(const std::string &x)
{
        std::string toret = StringMan::trim( x );
        StringMan::minsCnvt( toret );

        std::string::iterator it = toret.begin();

        for(; it != toret.end(); ++it) {

            // Eliminar chars innecesarios
            if ( !std::isalnum( *it ) )
            {
                toret.erase( it );
                *it = StringMan::maysCnvtCh( *it );
            }
        }

        toret[ 0 ] = StringMan::maysCnvtCh( toret[ 0 ] );

        return toret;
}

const std::string &Item::getIdUnico()
{
    if ( idUnico.empty() ) {
        idUnico = cnvtId( getId() );
    }

    return idUnico;
}

// ================================================================== ListaItems
void ListaItems::eliminar() {
        MapaItems::iterator it = items.begin();

        while( it != items.end() ) {
                delete ( it->second );
                ++it;
        }

        items.clear();
}

Item * ListaItems::insertaItem(Item *item) throw (ErrorSemantico, ErrorInterno)
{
        // Comprobar que no es NULL
        if ( item == NULL ) {
                throw ErrorInterno( "Tratando de almacenar valor nulo" );
        }

        // Comprobar que no existe ya
        if ( buscaItem( item->getId() ) ) {
                throw ErrorSemantico( item->getId() + " repetido" );
        }

        // Es la primera?
        if ( items.empty() ) {
                item->resetNumItems();
        }

        // Insertarla
        items.insert(
                MapaItems::value_type( item->getId(), item )
        );

        return item;
}

// ====================================================================== Objeto
const std::string Objeto::PrefijoObjeto = "obj";
const std::string Objeto::cmdCOGER      = " COGE COGER COJO TAKE GET PICK ";
const std::string Objeto::cmdEX         = " X EX EXAMINAR EXAMINA EXAMINO LOOK EXAMINE ";
const std::string Objeto::cmdPONER      = " PON PONTE PONER WEAR PUT ";
const std::string Objeto::cmdINVENTARIO = " I INVEN INV INVENTARIO INVENTORY ";

Objeto::Objeto(const std::string &id, Localidad *l, const std::string & desc)
        : Item(id, desc), escenario(false), ponible(false), llevado(false), continente(l)
{
    init();
}

Objeto::Objeto(const std::string &id, const std::string &loc, const std::string & desc)
        :   Item(id, desc), escenario(false), ponible(false), llevado(false),
            strContinente(loc), continente(NULL)
{
    init();
}

void Objeto::init()
{
        size_t pos = 0;
        size_t ultimaLetra;

        // Nombre de referencia del objeto
        nombreVoc = Scanner::getToken( getId(), pos );
        StringMan::minsCnvt( StringMan::normCnvt( nombreVoc ) );
        ultimaLetra = nombreVoc.length() - 1;

        // Es plural ?
        if ( StringMan::maysCnvtCh( nombreVoc[ultimaLetra] ) == 'S' )
                plural = true;
        else    plural = false;

        // Es femenino ?
        ultimaLetra -= plural ? 1 : 0;  // si es plural, es la segunda restante

        if ( StringMan::maysCnvtCh( nombreVoc[ultimaLetra] ) == 'A' )
                femenino = true;
        else    femenino = false;
}

const std::string &Objeto::getIdUnico()
{
        if ( idUnico.empty() ) {
            idUnico = PrefijoObjeto + Item::getIdUnico();
        }

        return idUnico;
}

Objeto * Objeto::recuperar(FILE *f)
{
        std::string marca;
        std::string nombre;
        std::string desc;
        std::string continente;
        Objeto * toret  = NULL;
        bool llevado    = false;
        bool escenario  = false;
        bool prenda     = false;

        if ( StringMan::mays( getSigMarca( f ) ) == "OBJ" ) {

            leeAperturaMarca( f, "OBJ" );

            marca = StringMan::mays( getSigMarca( f ) );
            while( marca != "OBJ"
              && !feof( f ) )
            {
                // Leer
                if ( StringMan::mays( getSigMarca( f ) ) == "NOMBRE" ) {
                    nombre = recuperarCampo( f, "NOMBRE" );
                }
                else
                if ( StringMan::mays( getSigMarca( f )  ) == "DESC" ) {
                    desc = recuperarCampo( f, "DESC" );
                }
                else
                if ( StringMan::mays( getSigMarca( f )  ) == "ESTAEN" ) {
                    continente = recuperarCampo( f, "ESTAEN" );
                }
                else
                if ( StringMan::mays( getSigMarca( f ) ) == "LLEVADO" ) {
                    llevado = ( StringMan::maysCnvtCh(
                            StringMan::lTrim(
                                recuperarCampo( f, "LLEVADO" ) )[ 0 ] ) == 'S' )
                    ;                }
                else
                if ( StringMan::mays( getSigMarca( f ) ) == "ESCENARIO" ) {
                    escenario = ( StringMan::maysCnvtCh(
                            StringMan::lTrim(
                                recuperarCampo( f, "ESCENARIO" ) )[ 0 ] ) == 'S' )
                    ;
                }
                else
                if ( StringMan::mays( getSigMarca( f ) ) == "PRENDA" ) {
                    prenda = ( StringMan::maysCnvtCh(
                            StringMan::lTrim(
                                recuperarCampo( f, "PRENDA" ) )[ 0 ] ) == 'S' )
                    ;
                }
                else recuperarCampo( f, marca );

                marca = StringMan::mays( getSigMarca( f ) );
            }
        }

        leeCierreMarca( f, "OBJ" );

        if ( !nombre.empty()
          && ( ( llevado || ( !continente.empty() ) ) && !( llevado && ( !continente.empty() ) ) )
        ) {
            toret = new Objeto( nombre, continente, desc );

            if ( escenario ) {
                toret->ponEstatico();
            }

            if ( llevado ) {
                toret->ponLlevado();
            }

            if ( prenda ) {
                toret->ponPonible();
            }
        }

        return toret;
}

void Objeto::guardar(std::FILE *f)
{
        std::ostringstream cabDir;
        escribeAperturaMarca( f, "Obj" );

        // Guardar la info de la loc
        guardarCampo( f, "Nombre", getId() );
        guardarCampo( f, "Desc", getDesc() );

        // Lugar del objeto
        if ( !esLlevado() )
                guardarCampo( f, "estaEn", getContinente()->getId() );
        else    guardarCampo( f, "Llevado", "S" );

        // Cosas del objeto
        if ( esEscenario() ) {
            guardarCampo( f, "Escenario", "S" );
        }
        else
        if ( esPlural() ) {
            guardarCampo( f, "Plural", "S" );
        }
        else
        if ( esFemenino() ) {
            guardarCampo( f, "Femenino", "S" );
        }
        else
        if ( esPonible() ) {
            guardarCampo( f, "Prenda", "S" );
        }

        escribeCierreMarca( f, "Obj" );
}

std::string Objeto::toString() const
{
        std::string toret = getId() + "( ";

        if ( esEscenario() ) {
            toret += "stc ";
        }

        if ( esLlevado() ) {
            toret += "carr ";
        }

        if ( esPonible() ) {
            toret += "w ";
        }

        toret += ") : ";
        toret += getDesc();

        return toret;
}

// =================================================================== Localidad
const std::string Localidad::strDireccion[] = {
        "NORTE", "SUR", "ESTE", "OESTE", "ARRIBA", "ABAJO",
        "NORDESTE", "SUDOESTE",
        "NOROESTE", "SUDESTE",
        "ADENTRO", "AFUERA",
        "LIMBO"
};

const std::string Localidad::PrefijoLocalidad = "loc";
const std::string Localidad::strNorte  = " N NORTE NORTH ";
const std::string Localidad::strSur    = " S SUR SOUTH ";
const std::string Localidad::strEste   = " E ESTE EAST ";
const std::string Localidad::strOeste  = " O OESTE W WEST ";
const std::string Localidad::strArriba = " SUBIR ARRIBA SUBE SUBO U UP ";
const std::string Localidad::strAbajo  = " BAJAR ABAJO BAJA BAJO DOWN D ";
const std::string Localidad::strNO     = " NOROESTE NO NORTHWEST NW ";
const std::string Localidad::strNE     = " NORESTE NE NORDESTE NORTHEAST ";
const std::string Localidad::strSO     = " SUROESTE SO SUDOESTE SOUTHWEST SW ";
const std::string Localidad::strSE     = " SUDESTE SE SURESTE SOUTHEAST ";
const std::string Localidad::strENTRA  = " ADENTRO DENTRO ENTRA ENTRO ENTRAR ENTER IN INSIDE ";
const std::string Localidad::strSAL    = " SALIR SAL SALGO AFUERA FUERA OUT OUTSIDE ";
const std::string Localidad::strPreps  = " A AL HACIA PARA CON TO TOWARDS ";
const std::string Localidad::cmdFinLoc = " .FINLOC .ENDROOM .FIN .END ";
const std::string Localidad::cmdGrf    = " .RECURSO_GRAFICO .GRAPHIC_RESOURCE .GRF .GRAPH ";
const std::string Localidad::cmdMsc    = " .RECURSO_SONIDO .MUSIC_RESOURCE .MSC ";


std::string Localidad::cnvtDireccionAStr(Direccion d)
{
        std::string toret = strDireccion[LIMBO];

        if ( d <= LIMBO
          && d >= 0 )
        {
                toret = strDireccion[d];
        }

        return toret;
}

Localidad::Direccion Localidad::cnvtStrADireccion(const std::string &x)
{
        Direccion toret = LIMBO;
        std::string dir = x;

        StringMan::maysCnvt( dir );
        dir = ' ' + dir + ' ';

        if ( strNorte.find( dir ) != std::string::npos ) {
            toret = N;
        }
        else
        if ( strSur.find( dir ) != std::string::npos ) {
            toret = S;
        }
        else
        if ( strEste.find( dir ) != std::string::npos ) {
            toret = E;
        }
        else
        if ( strOeste.find( dir ) != std::string::npos ) {
            toret = O;
        }
        else
        if ( strArriba.find( dir ) != std::string::npos ) {
            toret = ARRIBA;
        }
        else
        if ( strAbajo.find( dir ) != std::string::npos ) {
            toret = ABAJO;
        }
        else
        if ( strNE.find( dir ) != std::string::npos ) {
            toret = NE;
        }
        else
        if ( strNO.find( dir ) != std::string::npos ) {
            toret = NO;
        }
        else
        if ( strSE.find( dir ) != std::string::npos ) {
            toret = SE;
        }
        else
        if ( strSO.find( dir ) != std::string::npos ) {
            toret = SO;
        }
        else
        if ( strENTRA.find( dir ) != std::string::npos ) {
            toret = ADENTRO;
        }
        else
        if ( strSAL.find( dir ) != std::string::npos ) {
            toret = AFUERA;
        }

        return toret;
}

Localidad::Localidad(const std::string &id, const std::string &desc)
        : Item(id, desc)
{
        // Borrar todas las salidas
        for(size_t i = 0; i < NumDirecciones; ++i) {
                salidas[ i ].erase();
        }
}

const std::string &Localidad::getIdUnico()
{
        if ( idUnico.empty() ) {
            idUnico = PrefijoLocalidad + Item::getIdUnico();
        }

        return idUnico;
}


void Localidad::ponSalida(Direccion d, const std::string &id)
/**
        Permitir dar una salida con una localidad por una determinada
        dir. desde esta localidad.
        El movimiento se permite si esa dir. no contiene ya un
        movimiento o si el movimiento contenido es el mismo.
*/
{
        if ( salidas[ d ] != id )
        {
            if ( salidas[ d ].empty() )
            {
                    salidas[ d ] = id;
            }
            else {
                    std::string msg = "colocando en " + getId() + " la dir. "
                                + cnvtDireccionAStr( d )
                                + " a '" + id + "' cuando en esa dir. "
                                  "ya existe un movimiento a '"
                                + salidas[ d ]
                                + '\''
                    ;
                    throw ErrorIdDuplicado( msg );
            }
        }
}

std::string Localidad::toString() const
{
        std::string toret = getId() + '\n';

        toret += getDesc();
        toret += '\n';

        for(size_t i = 0; i < NumDirecciones; ++i)
        {
                if ( !( (getSalidas())[i].empty() ) ) {
                        toret += strDireccion[i];
                        toret += ':';
                        toret += ' ';
                        toret += (getSalidas())[i];
                        toret += '\n';
                }
        }

        return toret;
}

Localidad::Direccion
Localidad::cogeDir(const std::string &linact, std::string & idLocalidad)
{
        Direccion toret = LIMBO;
        std::string dir;
        size_t pos = 1; // Pasamos ya del '>'

        if ( linact[0] == CHR_PROMPT )
        {
            Scanner::pasaEsp( linact, pos );

            dir = Scanner::getToken( linact, pos );

            toret = cnvtStrADireccion( dir );

            if ( toret != LIMBO )
            {
                    // Buscar ahora si nos lleva a otra localidad
                    Scanner::pasaEsp( linact, pos );
                    std::string prep = Scanner::getToken( linact, pos );
                    Scanner::pasaEsp( linact, pos );
                    idLocalidad = linact.substr( pos, linact.length() );
                    StringMan::trimCnvt( idLocalidad );

                    StringMan::maysCnvt( prep );

                    if ( strPreps.find( prep ) == std::string::npos ) {
                            idLocalidad.clear();
                    }
            }
        }

        return toret;
}

Localidad::Direccion Localidad::cnvtDirInversa(Direccion d)
{
        Direccion toret = LIMBO;

        if ( d != LIMBO )
        {
            if ( d == N
              || d == E
              || d == ARRIBA
              || d == NE
              || d == NO
              || d == ADENTRO )
            {
                    toret = ( (Direccion) ( d + 1 ) );
            }
            else {
                    toret = ( (Direccion) ( d - 1 ) );
            }
        }

        return toret;
}

void Localidad::ponDesc(const std::string &d)
/**
        Los comentarios aparecen dentro de las descripciones de las localidades
        como texto entre corchetes.
        Al meter la desc., metemos el comentario.
*/
{
        size_t posIni = 0;
        size_t posFin = 0;

        desc = d;
        StringMan::trim( desc );

        posIni = desc.find( '[' );

        // Hay comentario ?
        if ( posIni != std::string::npos )
        {
                // Buscar el final ...
                posFin = desc.find( ']', posIni );

                // Cargarlo (evitando los corchetes)
                comentario = desc.substr( posIni + 1, posFin - posIni - 1);

                // Borrar el comentario de la desc.
                desc.erase( posIni, posFin - posIni + 1 );
        }

        return;
}

void Localidad::guardar(FILE *f)
{
        std::ostringstream cabDir;
        escribeAperturaMarca( f, "Loc" );

        // Guardar la info de la loc
        guardarCampo( f, "Nombre", getId() );
        guardarCampo( f, "Desc", getDesc() );

        if ( !getComentario().empty() ) {
                guardarCampo( f, "Com", getComentario() );
        }

        // Guardar recursos gráfico-sonoros
        if ( !nombreRecGrafico.empty() ) {
                guardarCampo( f, "Grf", nombreRecGrafico );
        }

        if ( !nombreRecMusica.empty() ) {
                guardarCampo( f, "Snd", nombreRecMusica );
        }

        // Salidas ?
        for(size_t i = 0; i < NumDirecciones; ++i)
        {
                if ( !(getSalidas())[i].empty() ) {
                        guardarCampo( f,
                                      strDireccion[i],
                                      (getSalidas())[i]
                        );
                }
        }

        escribeCierreMarca( f, "Loc" );
}

Localidad * Localidad::recuperar(FILE *f)
{
        Localidad * toret;
        std::string txt;

        // Crear la localidad
        toret = new(std::nothrow) Localidad( "kk" );

        if ( toret == NULL ) {
                throw ErrorInterno(
                        "Sin memoria creando loc: "
                        + txt
                );
        }

        // Leer el fichero XML
        txt = StringMan::mays( getSigMarca( f ) );

        if ( txt == "LOC" )
        {
                leeAperturaMarca( f );

                txt = getSigMarca( f );

                while( StringMan::mays( txt ) != "LOC"
                    && !feof( f ) )
                {
                        if ( StringMan::mays( txt ) == "NOMBRE" ) {
                                toret->id = recuperarCampo( f, txt );
                        }
                        else
                        if ( StringMan::mays( txt ) == "DESC" )
                        {
                                toret->desc = recuperarCampo( f, txt );
                        }
                        else
                        if ( StringMan::mays( txt ) == "COM" )
                        {
                                toret->comentario = recuperarCampo( f, txt );
                        }
                        else
                        if ( StringMan::mays( txt ) == "GRF" )
                        {
                                toret->nombreRecGrafico =
                                        recuperarCampo( f, txt );
                        }
                        else
                        if ( StringMan::mays( txt ) == "SND" )
                        {
                                toret->nombreRecMusica =
                                        recuperarCampo( f, txt );
                        }
                        else {
                                // Es una dirección ?
                                Direccion direccion =
                                        cnvtStrADireccion(
                                                StringMan::mays( txt )
                                );

                                if ( direccion != LIMBO )
                                {
                                        ( toret->salidas )[direccion] =
                                                recuperarCampo( f, txt )
                                        ;
                                } else {
                                        leeAperturaMarca( f );

                                        while( !leeCierreMarca( f, txt ) ) {
                                                fgetc( f );
                                        }
                                }
                        }

                        txt = getSigMarca( f );
                }

                // Leer el /loc
                leeCierreMarca( f, txt );
        } else {
                delete toret;
                toret = NULL;
        }

        return toret;
}

// ========================================================================= TDS
TDS::TDS(const std::string &n) : maxChars(UINT_MAX), nombreAventura(n)
{}

TDS::~TDS()
{
        locs.eliminar();
        objs.eliminar();
}

Objeto * TDS::insertaObjeto(const std::string &n, const std::string &l)
{
        Localidad *loc = buscaLoc( l );
        if ( loc == NULL ) {
                throw ErrorSemantico( "Loc no existente (?): '" + l + '\'' );
        }

        Objeto * toret = new(std::nothrow) Objeto( n, loc );
        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria creando obj: '" + n + '\'' );
        }

        insertaObjeto( toret );

        return loc->insertaObjeto( toret );
}


Localidad * TDS::insertaLoc(Localidad *loc)
{
        locs.insertaItem( loc );

        if ( locs.getNumItems() == 1 ) {
                locComienzo = loc;
        }

        return loc;
}

void TDS::ponLocalidadInicial(const std::string &strLoc)
{
    Localidad * loc = buscaLoc( strLoc );

    if ( loc != NULL ) {
        locComienzo = loc;
    }
}

void TDS::guardar(FichSalida &f)
{
	guardar( f.getDescriptor() );
}

void TDS::guardar(FILE *f)
{
    Objeto * obj;
    Localidad * loc;
	std::ostringstream buffer;

	escribirCabecera( f );

	escribeAperturaMarca( f, "Mapa" );

	// Localidad inicial
	guardarCampo( f, "LocInic", locComienzo->getId() );

	// Escribir localidades
    escribeAperturaMarca( f, "Locs" );

    loc = getPriLoc();
    for(; loc != NULL && !esLocalidadFinal(); loc = getSigLoc()) {
        loc->guardar( f );
    }

    escribeCierreMarca( f, "Locs" );

    // Escribir objetos
    escribeAperturaMarca( f, "Objs" );

    obj = getPriObj();
    for(; obj != NULL && !esObjetoFinal(); obj = getSigObj()) {
        obj->guardar( f );
    }

    escribeCierreMarca( f, "Objs" );

	escribeCierreMarca(f, "Mapa");
}

TDS * TDS::recuperar(FILE *f)
{
        std::string marca;
        std::string strLocInicial;
        Localidad proxyLoc( "kk" );
        Objeto proxyObj( "kk", "kk", "kk" );
        TDS * toret = new(std::nothrow) TDS( "Aventura" );
        Localidad  * loc;
        Objeto * obj;

        if ( toret == NULL ) {
                throw ErrorInterno( "Sin memoria creando TDS" );
        }

        leerCabecera( f );

        if ( leeAperturaMarca( f, "Mapa" ) )
        {

            marca = StringMan::mays( getSigMarca( f ) );
            while( marca != "MAPA"
               && !feof( f ) )
            {

                if ( marca == "LOCINIC" ) {
                    strLocInicial = recuperarCampo( f, marca );
                }
                else
                if ( marca == "LOCS" ) {
                    leeAperturaMarca( f, "LOCS" );
                    loc = proxyLoc.recuperar( f );

                    while ( loc != NULL ) {
                        toret->insertaLoc( loc );

                        loc = proxyLoc.recuperar( f );
                    }

                    if ( !leeCierreMarca( f, "Locs" ) ) {
                        delete toret;
                        toret = NULL;
                        throw ErrorSintaxis( "falta cierre marca 'Locs'" );
                    }
                }
                else
                if ( marca == "OBJS" ) {
                    leeAperturaMarca( f, "OBJS" );

                    obj = proxyObj.recuperar( f );

                    while ( obj != NULL ) {
                        toret->insertaObjeto( obj );

                        obj = proxyObj.recuperar( f );
                    }

                    if ( !leeCierreMarca( f, "Objs" ) ) {
                        delete toret;
                        toret = NULL;
                        throw ErrorSintaxis( "falta cierre marca 'Objs'" );
                    }
                }

                marca = StringMan::mays( getSigMarca( f ) );
            }

            if ( !leeCierreMarca( f, "Mapa" ) ) {
                  delete toret;
                  toret = NULL;
                  throw ErrorSintaxis( "falta cierre marca 'Mapa'" );
            }
        } else {
                delete toret;
                toret = NULL;
                throw ErrorSintaxis( "fichero txm no contiene datos" );
        }

        // Enlazar objetos con localidades
        for(obj = toret->getPriObj(); !( toret->esObjetoFinal() ); obj = toret->getSigObj()) {
            if ( obj->getContinente() == NULL ) {
                if ( !obj->esLlevado() )
                {
                    Localidad * loc = toret->buscaLoc( obj->getStrContinente() );

                    obj->ponContinente( loc );
                }
                else {
                    // Ponerle cualquier localidad
                    obj->ponContinente( toret->getPriLoc() );

                    // Guardarlo como objeto de inventario
                    toret->getListaInventarioObj()->push_back( obj );
                }
            }
        }

        // Poner la localidad inicial
        if ( !strLocInicial.empty() ) {
            toret->ponLocalidadInicial( strLocInicial );
        }

        return toret;
}

TDS * TDS::cargar(FichEntrada &f)
{
	TDS proxy( "" );
    return proxy.recuperar( f.getDescriptor() );
}

void TDS::volcarXML(const std::string &nom)
{
        std::string n = StringMan::trim( nom );

        // Comprobar si es directorio
        if ( n[ n.length() - 1 ] == '/'
          || n[ n.length() - 1 ] == '\\' )
        {
            n += nombreAventura;
        }

        // Quitarle la ext
        n = StringMan::extraerArchivoSinExtDeNombreArchivo( n );

        // El fichero xml, uno de los objetivos
        FichSalida fichXml( n + extXML );
        guardar( fichXml );

        return;
}

void TDS::chk() throw (ErrorSemantico, ErrorInterno)
{
        size_t numLocs = 0;
        size_t numObjs = 0;
        Localidad * it = getPriLoc();
        Objeto *obj;

        // Comprobar inconsistencias
        porPantalla( "\nChk( " + nombreAventura + ',' + ' '
                + StringMan::toString( getNumLocs() ) + " locs, "
                + StringMan::toString( getNumObjs() ) + " objs ):\n"
        );

        // Chk rooms
        for(; it != NULL && !esLocalidadFinal(); it = getSigLoc())
        {
                ++numLocs;

                // Chk ln. desc
                if ( it->getDesc().length() > getMaxChars() ) {
                    throw ErrorSemantico( "En '"
                                + it->getId() + "': #chars desc="
                                + StringMan::toString( it->getDesc().length() )
                                + " > MaxChars="
                                + StringMan::toString( getMaxChars() )
                    );
                }

                // Chk salidas
                for(size_t i = 0; i < Localidad::NumDirecciones; ++i)
                {
                        if ( !( it->getSalidas()[i].empty() )
                          && buscaLoc( it->getSalidas()[i] ) == NULL )
                        {
                                throw ErrorSemantico( "En '"
                                        + it->getId()
                                        + "' por "
                                        + Localidad::cnvtDireccionAStr(
                                                (Localidad::Direccion) i )
                                        + ", no existe '"
                                        + it->getSalidas()[i]
                                        + '\''
                                );
                        }
                }
        }

        porPantalla( '\t' + StringMan::toString( numLocs ) + " locs ok.\n" );

        // Comprobar los objetos del inventario
        for(size_t i = 0; i < getListaInventarioStr()->size(); ++i) {
                obj = buscaObjeto( ( *getListaInventarioStr() )[ i ] );

                if ( obj == NULL ) {
                    throw ErrorSemantico( "El objeto de inventario '"
                            + ( *getListaInventarioStr() )[ i ]
                            + "' no ha sido definido en el juego"
                    );
                } else obj->ponLlevado();

                getListaInventarioObj()->push_back( obj );
        }

        // Chk ln. desc objs
        obj = getPriObj();
        for(; obj != NULL && !esObjetoFinal(); obj = getSigObj())
        {
                ++numObjs;

                // Chk ln. desc
                if ( obj->getDesc().length() > getMaxChars() ) {
                    throw ErrorSemantico( "En '"
                                + it->getId() + "': #chars desc="
                                + StringMan::toString( obj->getDesc().length() )
                                + " > MaxChars="
                                + StringMan::toString( getMaxChars() )

                    );
                }
        }

        porPantalla( '\t' + StringMan::toString( numObjs ) + " objs ok.\n\n" );

        if ( numObjs != getNumObjs()
          || numLocs != getNumLocs() )
        {
            throw ErrorInterno( "#locs(" + StringMan::toString( numLocs )
                                + ") <> " + StringMan::toString( getNumLocs() )
                                + "|| #objs(" + StringMan::toString( numObjs )
                                + ") <> " + StringMan::toString( getNumObjs() )
            );
        }

}

// ------------------------------------------------------------------------ main
void procError(const Error &e, Scanner *sce, Parser *p)
{
                std::ostringstream buffer;

                if ( sce != NULL ) {
                  buffer << std::setw( 7 )   << sce->devNumLinea()
                         << ':'       << ' ' << sce->getLineaAct() << '\n'
                         << std::setw( 7 )   << sce->devNumLinea() << ',' << ' ';
                }

                buffer << e.toString() << '.' << '\n';

                if (p != NULL) {
                        p->ponLogStr( buffer.str() );
                }

                porPantallaError( buffer.str() );
}

void mostrarSintaxis()
{
	porPantalla( nombre   );
	porPantalla( version  );
	porPantalla( "\n\n\n" );
    porPantalla( "Sintaxis: \n\ttxtmap [--CLEAN] [--QUIET] [-O<nomfich>] "
                     "[--MOTR] <nomfich>\n\t"
                 "txtmap [--LANG=<langcode>] [--PL=[<nombre_plugin>]] <nomfich>\n\t"
	             "txtmap [--PL?[<nombre_plugin>]]"
	);

    porPantalla("\nOpciones:"
		    "\n\tCLEAN hace que no se generen los ficheros de log"
		    "\n\tLANG permite especificar un idioma (ISO-639) <langcode>"
		    "\n\tQUIET obliga a que solo se generen mensajes de error"
		    "\n\tPL? proporciona una lista de todos los plugins"
		    "\n\tPL?xx proporciona info sobre el plugin xx"
		    "\n\tPL=xx escoge el plugin xx para ejecutarlo"
		    "\n\tO/o permite proporcionar un nombre de fichero\n"
		        "\t\tde salida alternativo al nombre por defecto"
            "\n\tMKDIR indica que lo proporcionado con /o es un directorio a crear."
		    "\n\n\tNo existen incompatibilidades entre las opciones,\n"
		         "\t\tni deben especificarse en un orden especial"
		    "\n\n")
	;
}

void eliminaGuionesPrecedentes(std::string &arg)
// Eliminar los guiones precedentes en argumentos de línea de comando.
{
        size_t guion = 0;

        while ( arg[guion] == '-' ) {
                ++guion;
        }

        if ( guion > 0 ) {
                if ( guion > 2 ) {
                        throw ErrorSintaxis(
                                "especifique las "
                                "opciones con un menos "
                                "o dos como maximo: "
                                + arg
                        );
                }

                arg.erase( 0, guion );
        }
}

void procesaAyudaPlugin(std::string &arg)
{
    arg = arg.substr( opAyudaPl.length(),
                      arg.length() );

    if ( !arg.empty() )
    {
      StringMan::normCnvt( arg );
      Plugin * pl = Plugin::buscaPlugin( arg );

      if ( pl != NULL )
            porPantalla( pl->getInfo() );
      else  porPantalla( "Plugin no encontrado: " + arg );
    }
    else {
      Plugin::ListaPlugins::const_iterator it;

      porPantalla( "\nListado de plugins:\n" );
      porPantalla( "---\n" );
      for( it = Plugin::listaPlugins.begin();
           it != Plugin::listaPlugins.end();
           ++it)
      {
            porPantalla( it->first );
            porPantalla( "\n" );
      }

      porPantalla( "---\n" );
    }

    porPantalla( "\n" );
}

void eliminarComillas(std::string &arg)
{
    // Eliminar las primeras comillas, si existen
    if ( arg[ 0 ] == '"' ) {
        arg.erase( 0, 1 );
    }

    // Eliminar las segundas comillas (al final), si existen
    if ( arg[ arg.length() - 1 ] == '"' ) {
        arg.erase( arg.length() - 1, 1 );
    }
}

void mkdir(const std::string &d)
{
    std::string dir = d;
    char ch = dir[ dir.length() -1 ];

    if ( ch == '/'
      || ch == '\\' )
    {
        dir.erase( d.length() -1 );
    }


    int retVal = std::system( std::string( "mkdir " + dir ).c_str() );

    return;
}

std::string procesaOpciones(int argc, char *argv[],
		    bool &modoLimpio,
		    bool &modoQuiet,
            Plugin ** plExe,
		    std::string &fichS,
		    size_t &maxChars,
		    Plugin::Lengua &lng)
/*
	Procesa todas las opciones que hayan sido incorporadas en la lin. de comando,
	y devuelve, cuando lo encuentra, el nombre del fichero de entrada
*/
{
    bool createDir = false;
	int argAct = 1;
	std::string arg;

	modoLimpio = false;
	modoQuiet  = false;

	while ( argAct < argc )
	{
	    // Preparar el argumento
	    arg = argv[ argAct ];
	    StringMan::trimCnvt( arg );
        eliminarComillas( arg );
        StringMan::maysCnvt( arg );

        // Empezar a buscar
		if ( arg[0] == '-' )
		{
            eliminaGuionesPrecedentes( arg );

			// Cuál es?
			if ( arg.length() >= opOutput.length()
			&& arg.compare( 0, opOutput.length(), opOutput ) == 0 )
			{
                arg = argv[ argAct ];
                StringMan::trimCnvt( arg );
                eliminarComillas( arg );
				fichS = arg.substr( opOutput.length() + 1, arg.length() );
			}
			else
			if ( arg.length() >= opLanguage.length()
			&& arg.compare( 0, opLanguage.length(), opLanguage ) == 0 )
			{
                arg = arg.substr( opLanguage.length(), arg.length() );
                lng = Plugin::strALengua( arg );

                if ( lng == Plugin::NoLanguage ) {
                    mostrarSintaxis();
                    throw ErrorSintaxis( "LANG=" + arg + '?' );
                }
			}
			else
			if ( arg.length() >= opClean.length()
			&& arg.compare(0, opClean.length(), opClean) == 0)
			{
				// No crea los ficheros de log
				modoLimpio = true;
			}
            else
			if ( arg.length() >= opMaxChars.length()
			&& arg.compare(0, opMaxChars.length(), opMaxChars ) == 0)
			{
				// Max. longitud de los textos
				maxChars = StringMan::toIntNum(
                        arg.substr( opMaxChars.length(), arg.length() )
                );
			}
			else
			if ( arg.length() >= 4
			&& arg.compare(0, opMotr.length(),opMotr ) == 0)
			{
				// Checreto, checreto
				porPantalla( "\ntxtMap Primavera 2014"
					"\n Viva JSPAWS!\n\n\n"
					)
				;
			}
			else
			if ( arg.length() >= opQuiet.length()
			&& arg.compare(0, opQuiet.length(), opQuiet) == 0)
			{
				// No muestra salida por pantalla
				modoQuiet = true;
			}
            else
			if ( arg.length() >= opAyudaPl.length()
			&& arg.compare(0, opAyudaPl.length(), opAyudaPl) == 0)
            {
                procesaAyudaPlugin( arg );
            }
            else
			if ( arg.length() >= opExePl.length()
			&& arg.compare(0, opExePl.length(), opExePl) == 0)
            {
                arg = arg.substr( opExePl.length(), arg.length() );
                StringMan::normCnvt( arg );
                *plExe = Plugin::buscaPlugin( arg );

                if ( *plExe == NULL ) {
                    mostrarSintaxis();
                    throw ErrorSintaxis( opExePl + arg + '?' );
                }
            }
            else
			if ( arg.length() >= opMkdir.length()
			&& arg.compare(0, opMkdir.length(), opMkdir) == 0)
            {
                createDir = true;
            }
			else {
				mostrarSintaxis();
				throw ErrorSintaxis( "opción desconocida '" +
						std::string(argv[argAct]) + '\'');
			}
		} else break;

		++argAct;
	}

	if ( argAct >= argc ) {
		mostrarSintaxis();
		throw ErrorSintaxis( "falta el nombre del archivo de entrada" );
	}

    arg = argv[ argAct ];
    StringMan::trimCnvt( arg );

    // Eliminar posibles comillas
    eliminarComillas( arg );

    // Designar el nombre del fichero de salida
    if ( fichS.empty() )
    {
        if ( createDir ) {
                throw ErrorSintaxis( "falta directorio de salida (-Oxxx)" );
        }

        fichS = StringMan::extraerArchivoSinExtDeNombreArchivo( arg );
    }

    // Directorio de salida
    if ( createDir ) {
        mkdir( fichS );
    }

	return arg;
}

void crearNombreAventura(std::string &nombreAventura)
{
    nombreAventura = StringMan::extraerNombreArchivoSinPathNiExt( nombreAventura );
    nombreAventura[ 0 ] = StringMan::maysCnvtCh( nombreAventura[ 0 ] );
}

int procesarAventura(int argc, char *argv[])
{
    Plugin *plExe;
    Scanner *sce         = NULL;
    Parser *p            = NULL;
  	int toret            = 0;
	bool modoLimpio      = false;
	bool eraXml          = false;
    TDS * tds            = NULL;
	std::string nombreFichEntrada;
	std::string nombreFichSalida;
	std::string nombreAventura;
    FichEntrada *fichEnt;
    Plugin::Lengua lng   = Plugin::ES;
    size_t maxChars = 0;

    try {
        if ( argc < 2 ) {
            mostrarSintaxis();
        }
        else {
            // Presenta
            porPantalla( "\n" );
            porPantalla( nombre );
        	porPantalla( version );
        	porPantalla( "\n\n\n" );

            // Preparar plugins
            Plugin::iniciaListaPlugins();

            // Escoger el plugin por defecto
            plExe = Plugin::buscaPlugin( VoidPlugin::Nombre );

            if ( plExe == NULL ) {
                throw ErrorInterno( "plugin nulo no encontrado" );
            }

            // Procesar las opciones
            nombreFichEntrada = procesaOpciones(
                    argc, argv,
					modoLimpio,
					modoQuiet,
                    &plExe,
					nombreFichSalida,
					maxChars,
					lng
            );

            // Crear el nombre de la aventura
            crearNombreAventura( ( nombreAventura = nombreFichEntrada ) );

            // Mostrar opciones
            porPantalla( opClean + ':' + ( modoLimpio? "On ":"Off " ) );
            porPantalla( opQuiet + ':' + ( modoQuiet? "On ":"Off " ) );
            porPantalla( opExePl + StringMan::mins( plExe->getNombre() ) + ' ' );
            porPantalla( opLanguage + Plugin::lenguaACadena( lng )
                                + '(' + Plugin::lenguaAOpcionInform( lng ) + ')'
            );

            // Mostrar maxchars o darle valor max.
            if ( maxChars > 0 )
                    porPantalla( ' ' + opMaxChars + StringMan::toString( maxChars ) );
            else    maxChars = UINT_MAX;

            porPantalla( "\n" );

            // Mostrar nombre de Fichero
            porPantalla( "\nf  : " );

                  // Es un fichero XML o no ?
                  fichEnt = new FichEntrada( nombreFichEntrada );
                  if ( !fichEnt->estaAbierto()
                    || !Persistente::leerCabecera( fichEnt->getDescriptor() ) )
                  {
                        delete fichEnt;
                        fichEnt = NULL;

                        porPantalla( nombreFichEntrada + " (natural)");
                        porPantalla( "\n\n" );

                        // Preparar el scanner
                        sce = new(std::nothrow) Scanner( nombreFichEntrada );

                        if (  sce == NULL
                         ||!( sce->preparado() ) )
                        {
                              porPantalla("ERROR El archivo no existe o no está "
                                            "disponible.\n");
                        }
                        else {
                              p = new ParserAvNat( sce, modoLimpio );

                              // Procesar la entrada
                              p->procEntrada();

                              tds = ((ParserAvNat *) p)->getTDS();
                        }
                  } else {
                        porPantalla( nombreFichEntrada + " (xml)" );
                        porPantalla( "\n\n" );
                        fichEnt->rewind();
                        tds = TDS::cargar( *fichEnt );
                        eraXml = true;
                        delete fichEnt;
                        fichEnt = NULL;
                  }

                  // Chk
                  tds->nombreAventura = nombreAventura;
                  tds->ponMaxChars( maxChars );
                  tds->chk();

                  if ( tds->getNumLocs() > 0 ) {
                      // Procesar por el plugin
                      plExe->ponMaxChars( maxChars );
                      plExe->ponLengua( lng );
                      plExe->nomFichSal = nombreFichSalida;
                      plExe->procesar( tds );

                      // Volcar el xml si no estamos en modo limpio,
                      // y si el archivo de entrada no era Xml
                      if ( !modoLimpio
                        && !eraXml )
                      {
                              tds->volcarXML( nombreFichSalida );
                      }

                      porPantalla( "Ok, EOF(" + plExe->nomFichSal + ")\n" );
                  }
                  else porPantalla( "Ok, EOF()\n" );

                  // Limpiar y terminar
                  if ( p != NULL ) {
                        delete p;
                        delete sce;
                  } else {
                        delete tds;
                  }
          }
        }
        catch(const std::bad_alloc &e)
        {
            toret = -1;
            porPantalla("\n\nERROR FATAL: No hay memoria suficiente\n\n" );
        }
        catch(const Error &e) {
                toret = -1;
                procError(e, sce, p);
        }
        catch(const std::exception &e)
        {
            toret = -1;
            porPantalla( std::string( "\n\nERROR: " ) + e.what() );
        }
        catch(...) {
            toret = -1;
            porPantalla("\n\n*** FATAL: ERROR interno. "
                            "Contacte con el autor en baltasarq@yahoo.es\n\n");
        }

  	return toret;
}

// ------------------------------------------- Dependientes
#ifdef __TEXT__UI
void muestraProceso(Scanner *, Parser *) {
}

void porPantalla(const std::string &s)
{
	if ( !modoQuiet ) {
        	std::cout << s;
	}
}

void porPantallaError(const std::string &s)
{
	std::cerr << s;
}

int main(int argc, char *argv[]) {
        return procesarAventura(argc, argv);
}
#endif
