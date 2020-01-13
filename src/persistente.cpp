// persistente.cpp
/*
        Implementación de persistencia
		20060913

		Es la de la comunicación con el disco duro ;-)
*/

#include "persistente.h"
#include "stringman.h"

#include <cstdio>

// ================================================================= AtributoXML
std::unique_ptr<AtributoXML> AtributoXML::leerAtributo(std::FILE *f)
{
	long int pos = ftell( f );
	int ch;
	std::string etq;
	std::string val;
	std::unique_ptr<AtributoXML> toret( nullptr );

	// Leer la etiqueta
	Persistente::pasaEsp( f );
	etq = Persistente::getToken( f );

	if ( !etq.empty() ) {
			// Leer el '='
			Persistente::pasaEsp( f );
			ch = fgetc( f );

			if ( ch == '=' ) {
				// Leer las comillas
				Persistente::pasaEsp( f );
				ch = fgetc( f );

				if ( ch == '"' ) {
					// Leer el valor
					val = Persistente::leeLiteral( f );

					// Leer las comillas de cierre
					ch = fgetc( f );
					if ( ch == '"' ) {
						toret.reset( new(std::nothrow) AtributoXML( etq, val ) );
						if ( toret.get() == NULL ) {
							fseek( f, pos, SEEK_SET );
						}
					}
					else fseek( f, pos, SEEK_SET );
				}
				else fseek( f, pos, SEEK_SET );
			}
			else fseek( f, pos, SEEK_SET );
	} else fseek( f, pos, SEEK_SET );

	return toret;
}

// =========================================================== ListaAtributosXML
ListaAtributosXML Persistente::lAtribs;

bool ListaAtributosXML::leerAtributos(std::FILE *f)
{
	std::unique_ptr<AtributoXML> atr( AtributoXML::leerAtributo( f ) );

	reset();
	while ( atr.get() != NULL ) {
		carga( *atr );

		atr = AtributoXML::leerAtributo( f );
	}

	return ( getNumero() > 0 );
}

// ================================================================= Persistente
bool Persistente::leerCabecera(std::FILE *f)
{
        bool toret = false;
        char buffer[1024];
        char * retval;

        fseek(f, 0, SEEK_SET);
        retval = fgets(buffer, 1023, f);

        if ( retval != NULL ) {
            if (buffer[0] == '<'
             && buffer[1] == '?'
             && buffer[2] == 'x'
             && buffer[3] == 'm'
             && buffer[4] == 'l')
            {
                    toret = true;
            }
            else fseek(f, 0, SEEK_SET);
        }

        return toret;
}

void Persistente::pasaEsp(std::FILE *f)
{
        int ch;

        // Pasar los espacios
        ch = fgetc(f);
        while(ch != EOF
          &&  ( ch == ' '
           || ch == '\t'
           || ch == '\n'
           || ch == '\r' ))
        {
                ch = fgetc(f);
        }

        // Hemos tenido que leer el caracter para saber cuando parar
        fseek(f, -1, SEEK_CUR);
}

std::string Persistente::getToken(std::FILE *f)
{
        int ch;
        std::string toret;

        pasaEsp( f );

		ch = fgetc(f);

        while( ch != EOF
		   && ( std::isalpha( ch )
		     || std::isdigit( ch ) ) )
        {
                toret += ch;
                ch = fgetc(f);
        }

		// Hemos tenido que leer el caracter para saber cuando parar
        fseek(f, -1, SEEK_CUR);

        return toret;
}

std::string Persistente::leeLiteral(std::FILE *f, char delim)
{
		std::string toret;
		int ch = fgetc( f );

		while( ch != EOF
           &&  ch != delim )
        {
                toret += ch;
                ch = fgetc( f );
        }

		// Hemos tenido que leer el caracter para saber cuando parar
        fseek( f, -1, SEEK_CUR );

        return toret;
}

void Persistente::escribirCabecera(std::FILE *f)
{
        fprintf( f, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n\n" );
}

void Persistente::escribeAutoMarca(std::FILE *f, const std::string &nombre)
{
		std::string atrs = getAtributos().toString();

        fprintf( f, "<%s ", nombre.c_str() );

		if ( !atrs.empty() ) {
			lAtribs.guardaListaAtributos( f );
			lAtribs.reset();
		}

		fprintf( f, "/>\n" );
}

void Persistente::escribeAperturaMarca(std::FILE *f, const std::string &nombre)
{
		std::string atrs = getAtributos().toString();

        fprintf( f, "<%s ", nombre.c_str() );

		if ( !atrs.empty() ) {
			lAtribs.guardaListaAtributos( f );
			lAtribs.reset();
		}

		fprintf( f, ">\n" );
}

void Persistente::escribeCierreMarca(std::FILE *f, const std::string &nombre)
{
        fprintf(f, "</%s>\n\n", nombre.c_str());
}

std::string Persistente::leeAperturaMarca(std::FILE *f)
/**
        Lee una marca de apertura y devuelve la etiqueta de esa marca, o una
        cadena vacía si no existe tal marca
*/
{
        std::string toret;
        size_t pos;
        int ch;

        pasaEsp(f);
        pos = ftell(f);
        ch  = fgetc(f);

        if (ch != EOF
         && ch == '<')
        {
                // Leer el nombre de la etiqueta, y que no empiece por barra
                ch = fgetc(f);
                if (ch != '/')
                {
					// Leer la etiqueta
					fseek( f, -1, SEEK_CUR );
					toret = getToken( f );

					// Leer los atributos
					lAtribs.leerAtributos( f );

					// Leer el '>' final
					pasaEsp( f );
					ch = fgetc(f);
                	if (ch != '>')
                	{
						toret.clear();
						fseek(f, pos, SEEK_SET);
					}
                } else fseek(f, pos, SEEK_SET);
        } else fseek(f, pos, SEEK_SET);

        return toret;
}

std::string Persistente::getSigMarca(std::FILE *f)
/**
        Lee una marca de apertura y devuelve la etiqueta de esa marca, o una
        cadena vacía si no existe tal marca.
		El puntero vuelve a donde estaba (si se desea avanzar, hay que releer).
*/
{
        std::string toret;
        size_t pos;
        int ch;

        pasaEsp(f);

        pos = ftell(f);
        ch  = fgetc(f);

        if (ch != EOF
         && ch == '<')
        {
                // Leer el nombre de la etiqueta con barra o no
				ch = fgetc( f );
				if ( ch != '/'
				  && ch != EOF )
				{
					fseek( f, -1, SEEK_CUR );
				}

				toret = getToken( f );
        }

        // Volver a donde estábamos
        fseek(f, pos, SEEK_SET);

        return StringMan::trim( toret );
}

bool Persistente::leeAperturaMarca(std::FILE *f, const std::string &nombre)
{
        std::string maysNombre = StringMan::mays( StringMan::trim( nombre ) );
	    std::string marca = leeAperturaMarca( f );

		StringMan::maysCnvt( StringMan::trimCnvt( marca ) );

		return ( marca == maysNombre );
}

bool Persistente::leeCierreMarca(std::FILE *f, const std::string &nombre)
{
        size_t pos;
        bool toret = false;
        std::string nom;
        std::string maysNombre = StringMan::mays( StringMan::trim( nombre ) );
        int ch;

        pasaEsp(f);
        pos = ftell(f);
        ch  = fgetc(f);

        if ( ch != EOF
         &&  ch == '<' )
        {
                // Leer el nombre de la etiqueta
				pasaEsp( f );
                ch = fgetc(f);

				if ( ch == '/' )
				{
					nom = getToken( f );

					// ¿Es la que esperábamos?
					StringMan::maysCnvt( StringMan::trimCnvt( nom ) );
					if ( nom == maysNombre )
					{
					    pasaEsp( f );
						ch = fgetc( f );

						if ( ch == '>' ) {
							   toret = true;
						} else fseek( f, pos, SEEK_SET );
					}
					else fseek( f, pos, SEEK_SET );
				} else fseek( f, pos, SEEK_SET );
        }

        return toret;
}


void Persistente::guardarCampo(std::FILE *f, const std::string &nombre, const std::string &info)
{
        fprintf(f, "<%s>%s</%s>\n", nombre.c_str(), info.c_str(), nombre.c_str());
}

std::string Persistente::recuperarCampo(std::FILE *f, const std::string &nombre)
{
        int pos = ftell( f );
        int ch;
        std::string toret = "";

		if ( leeAperturaMarca( f, nombre ) )
		{
			// Leer el contenido
			toret = "";
			ch = fgetc(f);
			while(ch != EOF
			  &&  ch != '<')
			{
					toret += ch;
					ch = fgetc(f);
			}

			fseek( f, -1, SEEK_CUR );

			if ( !leeCierreMarca( f, nombre ) )
			{
				toret.clear();
				fseek(f, pos, SEEK_SET);
			}
		}
		else fseek(f, pos, SEEK_SET);

        return toret;
}
