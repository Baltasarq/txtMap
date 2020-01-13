// txtmap.h
/*
  	Cabecera
  	Este programa toma como entrada un texto como si de una aventura
	se tratara, y genera un fichero xml con la info.
*/

#ifndef __TXTMAP__H__
#define __TXTMAP__H__

#include <cstdio>
#include <string>
#include <map>
#include <stack>
#include <vector>
#include <stdexcept>

#include "persistente.h"

extern const std::string nombre;  // el nombre de la app
extern const std::string version; // el serial de la app

// ------------------------------------------------------ Excepciones

class Error : public std::runtime_error {
protected:
	std::string details;
    Error(const std::string &m, const std::string &d = "")
                : std::runtime_error(m), details(d) {}
public:
    virtual std::string toString() const;

	const std::string &getDetails() const { return details; }
  	Error() : std::runtime_error("indefinido") {}
	virtual ~Error() throw() {}
};

class ErrorSemantico : public Error {
public:
	ErrorSemantico(const std::string &d = "") : Error("semántico", d) {}
};

class ErrorIdDuplicado : public ErrorSemantico {
public:
	ErrorIdDuplicado(const std::string &d = "") : ErrorSemantico( d ) {}
};


class ErrorInterno : public Error {
public:
        ErrorInterno(const std::string &d = "") : Error("interno", d) {}
};

class ErrorES : public Error {
public:
        ErrorES(const std::string &d = "") : Error("entrada/salida", d) {}
};

class ErrorSintaxis : public Error {
protected:
	ErrorSintaxis(const std::string &msg, const std::string &d) : Error(msg, d) {}
public:
        ErrorSintaxis(const std::string &d = "") : Error("sintaxis", d) {}
};

// --------------------------------------------------------- Ficheros
class Fich {
  	protected:
    		std::FILE *f;

  	public:
    		static const bool LECTURA;
    		static const bool ESCRITURA;

    		Fich(const std::string &nomf, bool l_e) {
    	  		f = std::fopen( nomf.c_str(),  ( (l_e) ? "rt":"wt") );
    		}
    		~Fich() { if ( f != NULL ) std::fclose( f ); }

    		bool estaAbierto(void)        const
                { return (f != NULL ); }
    		bool esEof(void)              const
                { return std::feof( f ); }
            std::FILE * getDescriptor()   const
                { return f; }
};

class FichEntrada : public Fich {
  	private:
    		std::string buffer;
  	public:
    		FichEntrada(const std::string &f) : Fich(f, LECTURA) {}

                void rewind()
                        { fseek( f, 0, SEEK_SET ); }
                void irA(size_t x)
                        { fseek( f, x, SEEK_SET ); }
                void avanzar(int x)
                        { fseek( f, x, SEEK_CUR ); }
                void retrocederDesdeFin(size_t x)
                        { fseek( f, 0 - ((int) x), SEEK_CUR ); }

    		const std::string &leeLinea(void);
};

class FichSalida : public Fich {
  	public:
    		FichSalida(const std::string &nf) : Fich(nf, ESCRITURA) {}

    		void ponLinea(const std::string &l);
                static std::string toString(int);
};

// ------------------------------------------------------   El scanner
class Scanner {
  	protected:
    		std::string buf;
    		std::string nombre;
  	private:
    		FichEntrada *f;
    		size_t linea;
            size_t blank;

  	public:
            const static std::string DELIMITADORES;
            static const size_t MAXBUFFER = 16384;
            static void   pasaEsp(const std::string &, size_t &, const std::string &delim = DELIMITADORES);
            static std::string getToken(const std::string &, size_t &, const std::string &delim = DELIMITADORES);
            static std::string getNomFich(const std::string &);
            static std::string getLiteral(const std::string &, size_t &, char = '"');

    		Scanner(const std::string &);
    		~Scanner();

    		const std::string &leeLinea(void);

    		bool finEntrada(void) const { return f->esEof();       }
    		bool preparado(void)  const { return f->estaAbierto(); }
    		size_t devNumLinea(void) const { return linea;        }
            const std::string &getLineaAct() const { return buf; }

    		const std::string &devNombreEntrada(void) const { return nombre; }
            size_t devBlank(void)    const {return blank;}
};

// ------------------------------------------------------------------------- TDS
class Item : public Persistente {
protected:
        static size_t numItems;
        size_t numItem;
        std::string desc;
        std::string id;
        std::string idUnico;
public:
        // Max longitud id = 32 - ( sufijo obj + sufijo grf/msc)
        static const size_t MaxTamId = 25;

        Item(const std::string &i, const std::string &d = "");
        virtual ~Item();

        void ponDesc(const std::string &d)      { desc = d; }

        const std::string &getDesc()      const { return desc; }
        const std::string &getId()        const { return id;   }
        virtual const std::string &getIdUnico();
        size_t getNumItem()         const { return numItem; }
        void resetNumItem()                     { numItem = 1; }

        virtual std::string toString() const                             = 0;

        Item * recuperar(std::FILE *)                                    = 0;
        void guardar(std::FILE *)                                        = 0;

        static void chkId(const std::string &);
        static void resetNumItems()        { numItems = 1; }
        static std::string cnvtId(const std::string &);
        static size_t getNumItems()  { return numItems; }
};

class ListaItems {
public:
        typedef std::map<std::string, Item *> MapaItems;
private:
        MapaItems items;
        MapaItems::const_iterator it;
public:
        Item * insertaItem(Item *item);

        Item * buscaItem(const std::string &x)
                { MapaItems::iterator it = items.find( x );
                  return ( ( it == items.end() ) ? NULL : it->second );
                }

        Item * getPriItem()
                { return items.empty() ?
                        NULL : ( it = items.begin() )->second;
                }

        Item * getSigItem()
                { return items.empty() ? NULL : ( ++it )->second; }


        bool esPosFinal() const
                { return ( items.empty() || it == items.end() ); }

        size_t getNumItems() const
                { return items.size(); }

        void eliminar();
};

class Localidad;

class Objeto : public Item {
friend class TDS;
private:
        bool escenario;
        bool femenino;
        bool plural;
        bool ponible;
        bool llevado;
        std::string nombreVoc;
        std::string strContinente;
        Localidad * continente;

public:
        static const std::string PrefijoObjeto;
        static const std::string cmdCOGER;
        static const std::string cmdEX;
        static const std::string cmdPONER;
        static const std::string cmdINVENTARIO;

        Objeto(const std::string &id, Localidad *l, const std::string & desc = "");
        Objeto(const std::string &id, const std::string &loc, const std::string & desc = "");
        void init();

        bool esEscenario() const
                { return escenario; }

        const std::string &getStrContinente() const
                { return strContinente; }

        Localidad *getContinente() const
                { return continente; }

        void ponContinente(Localidad * loc)
                { continente = loc; }

        bool esPonible() const
                { return ponible; }

        bool esLlevado() const
                { return llevado; }

        const std::string &getIdUnico();

        void ponTransportable()
                { escenario = false; }
        void ponEstatico()
                { escenario = true;  }
        void ponPonible()
                { ponible = true; ponTransportable(); }
        void ponLlevado()
                { llevado = true; ponTransportable(); }

        std::string toString() const;

        const std::string &getNomVoc() const
                { return nombreVoc; }
        bool esPlural()   const
                { return plural; }
        bool esFemenino() const
                { return femenino; }

        size_t getNumObj() const
                { return getNumItem(); }

        static void resetNumObjs()
                { resetNumItems(); }

        static size_t getNumObjs() { return getNumItems(); }

	Objeto * recuperar(std::FILE *);
	void guardar(std::FILE *);
};

class Localidad : public Item  {
friend class TDS;
public:
    static const std::string PrefijoLocalidad;

    enum Direccion {
        N, S, E, O, ARRIBA, ABAJO,
        NE, SO, NO, SE, ADENTRO, AFUERA, LIMBO
    };
    static const std::string strDireccion[];
	static const size_t NumDirecciones = LIMBO;

    static const std::string strNorte;
    static const std::string strSur;
    static const std::string strEste;
    static const std::string strOeste;

    static const std::string strArriba;
    static const std::string strAbajo;

    static const std::string strNO;
    static const std::string strNE;
    static const std::string strSO;
    static const std::string strSE;
    static const std::string strENTRA;
    static const std::string strSAL;

    static const std::string strPreps;
    static const std::string cmdFinLoc;
    static const std::string cmdGrf;
    static const std::string cmdMsc;

private:
        std::string comentario;
        std::string salidas[NumDirecciones];
        ListaItems lobjs;
public:
        std::string nombreRecMusica;
        std::string nombreRecGrafico;
        size_t numId;

        Localidad(const std::string &id, const std::string &desc = "");

        void ponSalida(Direccion d, const std::string &id);

        void ponDesc(const std::string &d);
        const std::string &getComentario()const { return comentario; }
        const std::string* getSalidas()   const { return salidas; }
        size_t getNumLoc()     const { return getNumItem(); }

        std::string toString() const;

        Localidad * recuperar(std::FILE *);
        void guardar(std::FILE *);

        Objeto * insertaObjeto(Objeto *obj)
                { return (Objeto *) lobjs.insertaItem( obj ); }
        Objeto * buscaObjeto(const std::string &x)
                { return (Objeto *) lobjs.buscaItem( x ); }
        Objeto * getPriObj()
                { return (Objeto *) lobjs.getPriItem(); }
        Objeto * getSigObj()
                { return (Objeto *) lobjs.getSigItem(); }
        bool esObjetoFinal()
                { return  lobjs.esPosFinal(); }
        size_t getNumObjs() const
                { return lobjs.getNumItems(); }

        static Direccion cogeDir(const std::string &linact, std::string & idLocalidad);
        static std::string    cnvtDireccionAStr(Direccion);
        static Direccion cnvtStrADireccion(const std::string &);
        static Direccion cnvtDirInversa(Direccion);
        static void resetNumLocs()
                { resetNumItems(); }
        static size_t getNumLocs()
                { return getNumItems(); }

        const std::string &getIdUnico();
};

class TDS : public Persistente {
        public:
                typedef std::vector<std::string> ListaInventarioStr;
                typedef std::vector<Objeto *> ListaInventarioObj;
    	private:
                ListaItems objs;
                ListaItems locs;
                ListaInventarioStr listaInventarioStr;
                ListaInventarioObj listaInventarioObj;
                size_t maxChars;

                TDS * recuperar(std::FILE *);
                void guardar(std::FILE *);
        public:
                TDS() {}
                TDS(const std::string &nombre);
                ~TDS();

                size_t getMaxChars() const
                    { return maxChars; }

                void ponMaxChars(size_t chars)
                    { maxChars = chars; }

                Localidad *locComienzo;
                std::string nombreAventura;

                void ponLocalidadInicial(const std::string &strLoc);

                // Objetos ---------------------------------------------
                Objeto * insertaObjeto(const std::string &n, const std::string &l);
                Objeto * insertaObjeto(Objeto *obj)
                        { return (Objeto *) objs.insertaItem( obj ); }
                Objeto * buscaObjeto(const std::string &x)
                        { return (Objeto *) objs.buscaItem( x ); }
                Objeto * getPriObj()
                        { return (Objeto *) objs.getPriItem(); }
                Objeto * getSigObj()
                        { return (Objeto *) objs.getSigItem(); }
                bool esObjetoFinal()
                        { return  objs.esPosFinal(); }
                size_t getNumObjs() const
                        { return objs.getNumItems(); }

                // Localidades ------------------------------------------
                Localidad * insertaLoc(Localidad *loc);
                Localidad * buscaLoc(const std::string &x)
                        { return (Localidad *) locs.buscaItem( x ); }
                Localidad * getPriLoc()
                        { return (Localidad *) locs.getPriItem(); }
                Localidad * getSigLoc()
                        { return (Localidad *) locs.getSigItem(); }
                bool esLocalidadFinal()
                        { return  locs.esPosFinal(); }
                size_t getNumLocs() const
                        { return locs.getNumItems(); }

                // Otros --------------------------------------------------
                void guardar(FichSalida &fs);
                static TDS * cargar(FichEntrada &);

                void chk();
                void volcarXML(const std::string &);

                ListaInventarioStr * getListaInventarioStr()
                        { return &listaInventarioStr; }

                ListaInventarioObj * getListaInventarioObj()
                        { return &listaInventarioObj; }
};

// -------------------------------------------------------- El parser
class Parser {
protected:
    std::string nomFich;
    Scanner *scanSCE;
    FichSalida *fout;
    FichSalida *log;
    bool modoLimpio;

    void escribeSalida(const std::string &);
    void escribeSalidaParcial(const std::string &);

    Parser();
public:
    Parser(Scanner *, bool cl = false);
    virtual ~Parser();

    const std::string &getNomFich() const
        { return nomFich; }

    virtual bool cambiaEstado(const std::string &) = 0;
    virtual void procEntrada(void) = 0;
    void ponLogStr(const std::string &);

    bool enModoLimpio() const
        { return modoLimpio; }
};

class ParserAvNat : public Parser {
public:
        enum Estado {
                TOPLEVEL, TIT, DESC, GRF, MSC, MOV, OBJ,
                OBJDESC, OBJPONER, INVENTARIO, NADA, ERROR
        };
private:
        bool noCargar;
        Estado estado;
	TDS tds;
        std::string linact;
public:
        static const bool CapturaTexto = true;
        static const std::string strDeterminantes;

        ParserAvNat(Scanner *, bool cl = false);

        Estado getEstado() const { return estado; }

        void ignoraDeterminantes(const std::string &, size_t &);
        bool cambiaEstadoPorObjeto();
        bool cambiaEstado(const std::string &);
        void procEntrada(void);
        std::string buscaSigLineaCmd(bool = false);

        void crearNuevaLoc(Localidad *&, Localidad::Direccion &);
        void procesaMovimiento(Localidad *, Localidad::Direccion &);
        void procesaObjeto(Localidad *);
        void procesaObjetoEscenario(Localidad *);
        void procesaObjetoPonible(Localidad *);
        void procesaInventario(Localidad *);

        void cambiaEstado(Estado);

        TDS *getTDS() { return &tds; }
};

// Interfaz IU  ================================================================
extern void porPantalla(const std::string &);
extern void porPantallaError(const std::string &);
extern void muestraProceso(Scanner *, Parser *);
extern int  procesarAventura(int argc, char *argv[]);
extern void procError(const Error &e, Scanner *sce, Parser *p);

#endif

