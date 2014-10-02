// std::stringman.cpp
/*
        Clase que contiene funciones extras para el manejo
        de cadenas

        baltasarq@yahoo.es
*/

#include "stringman.h"
#include <cctype>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <algorithm>

const std::string StringMan::Espacios = " \t\r\n";
const std::string StringMan::CaracteresEspecialesMins = "áéíóúñüç";
const std::string StringMan::CaracteresEspecialesMays = "ÁÉÍÓÚÑÜÇ";
const std::string StringMan::CaracteresEspecialesMarcas = "¿¡";
const std::string StringMan::CaracteresEspecialesMinsNorm = "aeiounuc";
const std::string StringMan::CaracteresEspecialesMaysNorm = "AEIOUNUC";
const std::string StringMan::CaracteresEspecialesMarcasHtml[] = {
	"&iquest;",
	"&iexcl;"
};

const std::string StringMan::CaracteresEspecialesMinsHtml[] = {
	"&aacute;",
	"&eacute;",
	"&iacute;",
	"&oacute;",
	"&uacute;",
	"&ntilde;",
	"&ccedil;",
	"&uuml;"
};

const std::string StringMan::CaracteresEspecialesMaysHtml[] = {
	"&Aacute;",
	"&Eacute;",
	"&Iacute;",
	"&Oacute;",
	"&Uacute;",
	"&Ntilde;",
	"&Ccedil;",
	"&Uuml;"
};

// Quitar espacios anteriores y posteriores ====================================
std::string &StringMan::lTrimCnvt(std::string &x)
{
        std::string::iterator it = x.begin();

        while ( it != x.end()
			 && Espacios.find( *it ) != std::string::npos )
		{
                ++it;
        }

		x.erase( x.begin(), it );
        return x;
}

std::string &StringMan::rTrimCnvt(std::string &x)
{
		std::string::iterator it = x.end();

		--it;

        while ( it != x.begin()
			 && Espacios.find( *it ) != std::string::npos )
		{
                --it;
        }

		x.erase( ++it, x.end() );
        return x;
}

std::string &StringMan::trimCnvt(std::string &x)
{
        rTrimCnvt( x );
        lTrimCnvt( x );

        return x;
}

std::string StringMan::lTrim(const std::string &x)
{
        std::string toret = x;

        return lTrimCnvt( toret );
}

std::string StringMan::rTrim(const std::string &x)
{
        std::string toret = x;

        return rTrimCnvt( toret );
}

std::string StringMan::trim(const std::string &x)
{
        std::string toret = x;

        return trimCnvt( toret );
}

// Mayúsculas y minúsculas =====================================================
char StringMan::maysCnvtCh(char c, bool acentos)
{
        char toret = std::toupper( c );
		std::string::size_type pos = CaracteresEspecialesMins.find( toret );

		// Vocales acentuadas minúsculas
		if ( pos != std::string::npos ) {
			if ( acentos == MantenAcentos )
					toret = CaracteresEspecialesMays[ pos ];
			else	toret = CaracteresEspecialesMaysNorm[ pos ];
		}

		if ( acentos == EliminaAcentos ) {
			pos = CaracteresEspecialesMays.find( toret );

			// Vocales acentuadas mayúsculas
			if ( pos != std::string::npos ) {
				toret = CaracteresEspecialesMaysNorm[ pos ];
			}
		}

        return toret;
}

char StringMan::minsCnvtCh(char c, bool acentos)
{
        char toret = std::tolower( c );
		std::string::size_type pos = CaracteresEspecialesMays.find( toret );

		// Vocales acentuadas mayúsculas
		if ( pos != std::string::npos ) {
			if ( acentos == MantenAcentos )
					toret = CaracteresEspecialesMins[ pos ];
			else	toret = CaracteresEspecialesMinsNorm[ pos ];
		}

		if ( acentos == EliminaAcentos ) {
			pos = CaracteresEspecialesMins.find( toret );

			// Vocales acentuadas minúsculas
			if ( pos != std::string::npos ) {
				toret = CaracteresEspecialesMinsNorm[ pos ];
			}
		}

        return toret;
}

// ---------------------------------------------------------------------- mays()
std::string StringMan::mays(const std::string &x, bool acentos)
{
        std::string toret = x;

        return maysCnvt( toret, acentos );
}

// ------------------------------------------------------------------ maysCnvt()
std::string& StringMan::maysCnvt(std::string &x, bool acentos)
{
		std::string::iterator it = x.begin();
        for(; it != x.end(); ++it)
        {
               *it = maysCnvtCh( *it, acentos );
        }

        return x;
}

// ---------------------------------------------------------------------- mins()
std::string StringMan::mins(const std::string &x, bool acentos)
{
        std::string toret = x;

        return minsCnvt( toret, acentos );
}

// ------------------------------------------------------------------ minsCnvt()
std::string& StringMan::minsCnvt(std::string &x, bool acentos)
{
		std::string::iterator it = x.begin();
        for(; it != x.end(); ++it)
        {
                *it = minsCnvtCh( *it, acentos );
        }

        return x;
}

// ================================================================== Normalizar
// ---------------------------------------------------------------------- norm()
std::string StringMan::norm(const std::string &x)
{
        std::string toret = x;

        return normCnvt( toret );
}

// ------------------------------------------------------------------ normCnvt()
std::string &StringMan::normCnvt(std::string &x)
{
	std::string::iterator it;

	// Quitar los espacios
        trimCnvt( x );

	// Quitar los acentos y pasar a mayúsculas
        maysCnvt( x, EliminaAcentos );

	// Quitar cualquier carácter no imprimible
	for(it = x.begin(); it != x.end(); ++it) {
		if ( !std::isprint( *it ) ) {
			*it = CaracterEspecial;
		}
	}

	return x;
}

// --------------------------------------------- StringMan::cambiarFmtImprCnvt()
std::string &StringMan::cambiarFmtImprCnvt(std::string &x)
{
        size_t pos;

        // Quitar espacios
        trimCnvt( x );

        // Pasar a minúsculas
        minsCnvt( x );

        // Poner la primera en mayúsculas
        x[ 0 ] = maysCnvtCh( x[ 0 ] );

        // Poner las secundarias en mayúsculas (después de espacios)
        pos = x.find(' ');

        while ( pos != std::string::npos )
        {
                x[ pos + 1 ] = maysCnvtCh( x[ pos + 1 ] );

                pos = x.find( ' ', pos + 1 );
        }

        // Poner las secundarias en mayúsculas (después de guiones)
        pos = x.find( '-' );

        while ( pos != std::string::npos )
        {
                x[pos + 1] = maysCnvtCh( x[pos + 1] );

                pos = x.find( '-', pos + 1 );
        }

        return x;
}

// ------------------------------------------------- StringMan::cambiarFmtImpr()
std::string StringMan::cambiarFmtImpr(const std::string &x)
{
        std::string toret = x;

        return cambiarFmtImprCnvt( toret );
}

// ------------------------------------------ StringMan::cambiarCaracteresCnvt()
std::string &StringMan::cambiarCaracteresCnvt(std::string &x, char busc, char reem)
{
        std::string::size_type pos = x.find( busc );

        while( pos != std::string::npos ) {
                x[pos] = reem;

                pos = x.find( busc );
        }

        return x;
}

// ---------------------------------------------- StringMan::cambiarCaracteres()
std::string StringMan::cambiarCaracteres(const std::string &x, char busc, char reem)
{
        std::string toret = x;

        return cambiarCaracteresCnvt( toret, busc, reem );
}

// ================================================================ Conversiones
// ------------------------------------------------ StringMan::esNumeroDecimal()
bool StringMan::esNumeroDecimal(const std::string &strNum)
{
		std::string::const_iterator pos = strNum.begin();
		size_t numPuntos = 0;
		bool toret = false;

		// Revisión sintáctica
		while( pos != strNum.end() )
		{
			if ( *pos == '.' ) {
				++numPuntos;
			}
			else
			if ( *pos == '-'
			  && pos != strNum.begin() )
			{
				break;
			}
			else
			if ( *pos == '+'
			  && pos != strNum.begin() )
			{
				break;
			}
			else
			if ( !isdigit( *pos )
			  && *pos != '-'
			  && *pos != '+' )
			{
				break;
			}

			++pos;
		}

		// Sólo puede haber un punto, se tiene que haber revisado toda la cadena
		if ( numPuntos <= 1
		  && pos == strNum.end() )
		{
			toret = true;
		}

		return toret;
}

// ------------------------------------------------- StringMan::esNumeroEntero()
bool StringMan::esNumeroEntero(const std::string &strNum)
{
		bool toret = true;
		std::string::const_iterator pos = strNum.begin();

		// Revisión sintáctica
		while( pos != strNum.end() )
		{
			if ( !isdigit( *pos ) ) {
				toret = false;
				break;
			}

			++pos;
		}

		return toret;
}


// ------------------------------------------------------- StringMan::toString()
std::string StringMan::toString(const int &x)
{
        std::ostringstream cnvt;

        cnvt << x;

        return cnvt.str();
}

// ------------------------------------------------------- StringMan::toString()
std::string StringMan::toString(const unsigned int &x)
{
        std::ostringstream cnvt;

        cnvt << x;

        return cnvt.str();
}

// ------------------------------------------------------- StringMan::toString()
std::string StringMan::toString(const double &x, int w, int d)
{
        std::ostringstream cnvt;

		if ( w == -1 )
	    		cnvt << x;
		else	cnvt << std::fixed << std::setw( w )
				     << std::setprecision( d ) << std::setfill( '0' ) << x;

        return cnvt.str();
}

// ------------------------------------------------------- StringMan::toString()
std::string StringMan::toString(const std::time_t &x)
{
        std::tm *tmHora = std::localtime( &x );

        return std::string( std::asctime( tmHora ) );
}

// ------------------------------------------------------- StringMan::toIntNum()
int StringMan::toIntNum(const std::string &x)
{
        return std::atoi( x.c_str() );
}

// ------------------------------------------------------- StringMan::toFltNum()
double StringMan::toFltNum(const std::string &x)
{
        return std::atof( x.c_str() );
}

// --------------------------------------------------------- StringMan::toHtml()
std::string StringMan::toHtml(const std::string &x)
{
	std::string toret = x;

	return toHtmlCnvt( toret );
}

// -------------------------------------------------------- StringMan::replace()
std::string StringMan::cambiarCadenas(
		const std::string &x,
		char c,
		const std::string &txt)
{
	std::string toret = x;

	return cambiarCadenasCnvt( toret, c, txt );
}

std::string &StringMan::cambiarCadenasCnvt(std::string &x, char c, const std::string &txt)
{
	std::string::iterator it;
	std::string::size_type pos = x.find( c );

	while( pos != std::string::npos ) {
		// Avanzar hasta la posición
		it = x.begin();
		std::advance( it, pos );

		// Reemplazar
		x.replace(it, it + 1, txt );

		// Siguiente búsqueda
		pos += txt.length();
		pos = x.find( c, pos );
	}

	return x;
}

std::string StringMan::cambiarCadenas(
		const std::string &x,
		const std::string &txt1,
		const std::string &txt2)
{
	std::string toret = x;

	return cambiarCadenasCnvt( toret, txt1, txt2 );
}

std::string &StringMan::cambiarCadenasCnvt(
					std::string &x,
					const std::string & txt1,
					const std::string & txt2)
{
	std::string::iterator it;
	std::string::size_type pos = x.find( txt1 );

	while( pos != std::string::npos ) {
		// Avanzar hasta la posición
		it = x.begin();
		std::advance( it, pos );

		// Reemplazar
		x.replace(it, it + txt1.length(), txt2 );

		// Siguiente búsqueda
		pos += txt2.length();
		pos = x.find( txt1, pos );
	}

	return x;
}

// ----------------------------------------------------- StringMan::toHtmlCnvt()
std::string &StringMan::toHtmlCnvt(std::string &x)
/*
	Es necesario recalcular los punteros, ya que al cambiar el tamaño
	de la cadena, es posible que ésta ya no esté en la antigua posición
	en memoria.
*/
{
	std::string::const_iterator it;

	// Las marcas
	for( it = CaracteresEspecialesMarcas.begin(); it < CaracteresEspecialesMarcas.end(); ++it) {
		cambiarCadenasCnvt( x,
				    *it,
				    CaracteresEspecialesMarcasHtml[
					it - CaracteresEspecialesMarcas.begin()
				    ]
		);
	}

	// Las minúsculas
	for( it = CaracteresEspecialesMins.begin(); it < CaracteresEspecialesMins.end(); ++it) {
		cambiarCadenasCnvt( x,
				    *it,
				    CaracteresEspecialesMinsHtml[
					it - CaracteresEspecialesMins.begin()
				    ]
		);
	}

	// Las mayúsculas
	for( it = CaracteresEspecialesMays.begin(); it < CaracteresEspecialesMays.end(); ++it) {
		cambiarCadenasCnvt( x,
				    *it,
				    CaracteresEspecialesMaysHtml[
					it - CaracteresEspecialesMays.begin()
				    ]
		);
	}

	return x;
}

// ------------------------------------------------ StringMan::esLexicoGraficamenteMenor()
bool StringMan::esLexicoGraficamenteMenor(const std::string &s1, const std::string &s2)
{
	bool toret = false;
	std::string::const_iterator it1 = s1.begin();
	std::string::const_iterator it2 = s2.begin();

	// Avanzar hasta el primero distinto
	while( it1 != s1.end()
		&& it2 != s2.end()
		&& ( *it1 == *it2
		  || !isprint( *it1 )
		  || !isprint( *it2 ) ) )
	{
		++it1, ++it2;
	}

	// Realizar la comparacic.
	if ( it1 != s1.end()
	  && it2 != s2.end() )
	{
		if ( *it1 < *it2 ) {
			toret = true;
		}
	}

	return toret;
}

// Operaciones sobre nombres de archivos
// --------------------------------- StringMan::ponerExtensionAdecuadaArchivo()
std::string &StringMan::ponerExtensionAdecuadaNombreArchivo(
    std::string &nomFichSal, const std::string &ext
)
{
    std::string extFich = nomFichSal.substr( nomFichSal.length() - ext.length() );

    // Dar la ext., si es necesario
    if ( extFich != ext ) {
            nomFichSal += ext;
    }

    return nomFichSal;
}

// --------------------------------------- StringMan::extraerExtensionArchivo()
std::string StringMan::extraerExtensionArchivo(const std::string &nombreArchivo)
{
    size_t pos;
    std::string toret;

    toret = extraerNombreArchivoSinPath( nombreArchivo );

    pos = toret.rfind( '.' );
    if ( pos != std::string::npos ) {
        toret = nombreArchivo.substr( pos + 1, nombreArchivo.length() );
    }

    return toret;
}

// ------------------------------ StringMan::extraerNombreArchivoSinPathNiExt()
std::string StringMan::extraerNombreArchivoSinPathNiExt(const std::string &nombreArchivo)
{
    size_t pos;
    std::string toret;

    toret = extraerNombreArchivoSinPath( nombreArchivo );

    pos = toret.rfind( '.' );
    if ( pos != std::string::npos ) {
        toret.erase( pos );
    }

    return toret;
}

// ----------------------------------- StringMan::extraerNombreArchivoSinPath()
std::string StringMan::extraerNombreArchivoSinPath(const std::string &nombreArchivo)
{
    size_t pos;
    std::string toret = nombreArchivo;

    pos = toret.rfind( '\\' );
    if ( pos != std::string::npos ) {
        toret =  toret.substr( pos + 1, toret.length() );
    }
    else {
        pos = toret.rfind( '/' );
        if ( pos != std::string::npos ) {
            toret =  toret.substr( pos + 1, toret.length() );
        }
    }

    return toret;
}

// ------------------------------------ StringMan::extraerPathDeNombreArchivo()
std::string StringMan::extraerPathDeNombreArchivo(const std::string &nombreArchivo)
{
    size_t pos;
    std::string toret = nombreArchivo;

    pos = toret.rfind( '\\' );
    if ( pos != std::string::npos ) {
        toret.erase( pos );
    }
    else {
        pos = toret.rfind( '/' );
        if ( pos != std::string::npos ) {
            toret.erase( pos );
        }
        else {
            toret.clear();
        }
    }

    return toret;
}

// --------------------------- StringMan::extraerArchivoSinExtDeNombreArchivo()
std::string StringMan::extraerArchivoSinExtDeNombreArchivo(const std::string &nombreArchivo)
{
    std::string toret = nombreArchivo;
    size_t posBarra = toret.rfind( '\\' );
    size_t posPunto = toret.rfind( '.' );

    if ( posBarra == std::string::npos ) {
        posBarra = toret.rfind( '/' );
    }

    if ( posBarra == std::string::npos ) {
        posBarra = 0;
    }

    if ( posPunto != std::string::npos ) {
        if ( posPunto > posBarra ) {
            toret.erase( posPunto );
        }
    }

    return toret;
}

size_t StringMan::split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

bool StringMan::compararIgnorandoMays(const std::string &s1, const std::string &s2)
{
    std::string str1 = mays( s1 );
    std::string str2 = mays( s2 );

    return ( s1.compare( s2 ) == 0 );
}
