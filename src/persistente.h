// persistente.cpp
/*
        Implementación de persistencia
		20060913
*/

#ifndef __PERSISTENTE__H
#define __PERSISTENTE__H

#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

class AtributoXML {
private:
        std::string etq;
        std::string valor;
public:
        AtributoXML(const std::string &x, const std::string &y) : etq( x ), valor( y )
                {}

        const std::string &getEtq() const
                { return etq; }

        const std::string &getValor() const
                { return valor; }

		std::string toString() const
				{ return ( getEtq() + '=' + '"' + getValor() + '"' ); }

		void guardaAtributo(FILE * f)
				{ std::fprintf( f, "%s", toString().c_str() ); }

		static std::auto_ptr<AtributoXML> leerAtributo(FILE *);
};

class ListaAtributosXML {
private:
        std::vector<AtributoXML> la;
public:
        size_t getNumero() const
                { return la.size(); }

        const AtributoXML &getElemento(size_t i) const
                { return la[i]; }

        void reset()
                { la.clear(); }

        void carga(const AtributoXML &atr)
                { la.push_back( atr ); }

        void cargaAtribs(const ListaAtributosXML &l)
                {
                        for(size_t i = 0; i < l.getNumero(); ++i) {
                                carga( l.getElemento( i ) );
                        }
                }

        std::string toString() const
                {
                        std::string toret;

                        for(size_t i = 0; i < getNumero(); ++i) {
							toret += getElemento( i ).toString() + ' ';
                        }

                        return toret;
                }

		void guardaListaAtributos(FILE *f)
				{ std::fprintf( f, "%s", toString().c_str() ); }

		bool leerAtributos(FILE *);
};





class Persistente {
      private:
        static ListaAtributosXML lAtribs;
      public:
		virtual ~Persistente()
	  		{}

        static ListaAtributosXML &getAtributos()
                { return lAtribs; }


        static  void pasaEsp(FILE *);
        static  std::string getToken(FILE *);
		static  std::string leeLiteral(FILE *, char = '"');

		// Operaciones mínimas sobre XML
        static  void escribirCabecera(FILE *);
        static  bool leerCabecera(FILE *);
        static  std::string leeAperturaMarca(FILE *);
        static  std::string getSigMarca(FILE *f);
        static  void escribeAutoMarca(FILE *, const std::string &nombre);
        static  void escribeAperturaMarca(FILE *, const std::string &nombre);
        static  void escribeCierreMarca(FILE *, const std::string &nombre);
        static  bool leeAperturaMarca(FILE *, const std::string &nombre);
        static  bool leeCierreMarca(FILE *, const std::string &nombre);
        static  void guardarCampo(FILE *, const std::string &nombre, const std::string &info);
        static  std::string recuperarCampo(FILE *, const std::string &nombre);

        // Operaciones que deberán ser reescritas por cada clase derivada
        virtual Persistente * recuperar(FILE *)         = 0;
        virtual void guardar(FILE *)                    = 0;
};

#endif
