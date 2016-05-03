// std::stringman.h
/*
        Clase que contiene funciones extras
        para el manejo de cadenas

        baltasarq@yahoo.es
*/

#ifndef __STRINGMAN__H__
#define __STRINGMAN__H__

#include <vector>
#include <string>
#include <ctime>

class StringMan {
public:
        static const bool MantenAcentos = true;
        static const bool EliminaAcentos = false;

        static const char CaracterEspecial = '_';
        static const std::string Espacios;
		static const std::string CaracteresEspecialesMins;
		static const std::string CaracteresEspecialesMays;
		static const std::string CaracteresEspecialesMarcas;
		static const std::string CaracteresEspecialesMinsNorm;
		static const std::string CaracteresEspecialesMaysNorm;
		static const std::string CaracteresEspecialesMinsHtml[];
		static const std::string CaracteresEspecialesMaysHtml[];
		static const std::string CaracteresEspecialesMarcasHtml[];


        // Caracteres
        static char maysCnvtCh(char c,       bool = MantenAcentos);
        static char minsCnvtCh(char c,       bool = MantenAcentos);

        // May�sculas
        static std::string mays(const std::string &, bool = MantenAcentos);
        static std::string &maysCnvt(std::string &,   bool = MantenAcentos);

        // Min�sculas
        static std::string mins(const std::string &, bool = MantenAcentos);
        static std::string &minsCnvt(std::string &,   bool = MantenAcentos);

        /// Eliminar los espacios anteriores
        static std::string &lTrimCnvt(std::string &);
		/// @see lTrimCnvt
        static std::string lTrim(const std::string &);

        /// Eliminar los espacios posteriores
        static std::string &rTrimCnvt(std::string &);
		/// @see rTrimCnvt
        static std::string rTrim(const std::string &);

        static std::string &trimCnvt(std::string &);
        static std::string trim(const std::string &);

        // Conversiones de tipos
		static bool esNumeroDecimal(const std::string &);
		static bool esNumeroEntero(const std::string &);
        static std::string stringFromInt(const int &);
        static std::string stringFromUInt(const unsigned int &);
        static std::string stringFromDouble(const double &, int = -1, int = -1);
        static std::string stringFromTime(const std::time_t &);
        static int toIntNum(const std::string &);
        static double toFltNum(const std::string &);

        // Modificaciones
        static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch);
        static std::string &cambiarCaracteresCnvt(std::string &x, char busc, char reem);
        static std::string cambiarCaracteres(const std::string &x, char busc, char reem);
		static std::string &cambiarCadenasCnvt(std::string &x, char c, const std::string &txt);
		static std::string &cambiarCadenasCnvt(std::string &x, const std::string &, const std::string &);
		static std::string cambiarCadenas(const std::string &x, char c, const std::string &txt);
		static std::string cambiarCadenas(const std::string &x, const std::string &, const std::string &);

		/// @see toHtmlCnvt
		static std::string toHtml(const std::string &);

		/// Convertir los caracteres especiales en Html
		static std::string &toHtmlCnvt(std::string &);

        // Normalizar
		/// @see normCnvt
        static std::string norm(const std::string &);
		/**
        	Devuelve la "forma can�nica" de una cadena:
        	convertida en may�sculas y sin acentos. Otros caracteres como caracter especial
		@see CaracterEspecial
		*/
        static std::string &normCnvt(std::string &);

		/// Pone la cadena en formato de impresi�n
        static std::string &cambiarFmtImprCnvt(std::string &);

		/// @see cambiarFmtImprCnvt
        static std::string cambiarFmtImpr(const std::string &);

		/** Compara dos cadenas, s1 y s2 lexicogr�ficamente, sin importar el tama�o
		    @param s1 La primera cadena a comparar
			@param s2 la segunda cadena a comparar
			@return true si s1 es menor que s2, false en otro caso
		**/
		static bool esLexicoGraficamenteMenor(const std::string &s1, const std::string &s2);

		/**
            Compara dos cadenas, s1 y s2 lexicogr�ficamente, sin importar mays/mins
		    @param s1 La primera cadena a comparar
			@param s2 la segunda cadena a comparar
			@return true si s1 es igual que s2, false en otro caso
		*/
		static bool compararIgnorandoMays(const std::string &s1, const std::string &s2);

		// Archivos
		/** Pone la ext. pasada a un nombre de archivo, si no estaba ya
		    @param nomFichSal El nombre del archivo
			@param ext La ext. a poner
			@return Param. nomFichSal modificado
		**/
		static
		std::string &ponerExtensionAdecuadaNombreArchivo(std::string &nomFichSal, const std::string &ext);

		/** Extrae la ext. de un nombre de archivo
		    @param nombreArchivo El nombre del archivo
			@return Una cadena con la ext. del nombre del archivo pasado
		**/
		static
		std::string extraerExtensionArchivo(const std::string &nombreArchivo);

		/** Extrae el nombre de un archivo (no ext, no path)
		    @param nombreArchivo El nombre del archivo
			@return Una cadena con solo el nombre del archivo
		**/
		static
		std::string extraerNombreArchivoSinPathNiExt(const std::string &nombreArchivo);

		/** Extrae el nombre de un archivo (no ext, pero con path)
		    @param nombreArchivo El nombre del archivo
			@return Una cadena con el path + el nombre del archivo
		**/
		static
		std::string extraerNombreArchivoSinPath(const std::string &nombreArchivo);

		/** Extrae el path de un nombre de archivo
		    @param nombreArchivo El nombre del archivo
			@return Una cadena con el path
		**/
		static
		std::string extraerPathDeNombreArchivo(const std::string &nombreArchivo);

		/** Extrae el path de un nombre de archivo, todo excepto la ext.
		    @param nombreArchivo El nombre del archivo
			@return Una cadena con el path + el nombre, sin la ext.
		**/
		static
		std::string extraerArchivoSinExtDeNombreArchivo(const std::string &nombreArchivo);

		template <typename T>
        static const T * buscarEnVector(const T * v[], T x)
        {
            const T ** ptr = v;

            for(; *ptr != NULL; ++ptr) {
                if ( **ptr == x ) {
                    break;
                }
            }

            return ( ptr != NULL ? *ptr : NULL );
        }
};

#endif
