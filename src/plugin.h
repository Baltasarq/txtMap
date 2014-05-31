// plugin.h
/*
        Lleva todo lo relacionado con los Plugins.
*/

#ifndef PLUGIN__H__
#define PLUGIN__H__

#include <string>
#include <map>

#include "txtmap.h"
#include "persistente.h"

class Plugin {
public:
       /**
        La opc. language soporta los siguientes valores:
            German    DE
            English   EN
            Spanish   ES
            French    FR
            Italian   IT
            Dutch	  NL
            Slovenian SL
            Swedish   SV
        */
        enum Lengua {
            DE, EN, ES, FR, IT, NL, SL, SV, NoLanguage
        };

        /// @brief Versiones de cadena de las lenguas soportadas
        /// @see Lengua
        static const std::string StrLengua[];

        /// @brief Opciones de inform para las lenguas soportadas
        /// @see Lengua
        static const std::string OpcLenguaInform[];

        /// @see Lengua
        static const std::string &lenguaACadena(Lengua);
        /// @see Lengua
        static Lengua strALengua(const std::string &);
        /// @see Lengua
        static const std::string &lenguaAOpcionInform(Lengua);


        /// @brief La lista de todos los plugins soportados
        typedef std::map<std::string, Plugin *> ListaPlugins;
protected:
        std::string nombrePlugin;
        std::string autorPlugin;
        std::string versionPlugin;
        std::string helpPlugin;
        Lengua lengua;
        size_t maxChars;

        std::string &ponerExtensionAdecuada(std::string &, const std::string &);

        Plugin(const std::string &, const std::string &, const std::string &, const std::string &);
public:
        std::string nomFichSal;

        static std::string &procStr(std::string &, char, const std::string &);
        static std::string procStr(const std::string &str, char c, const std::string &cnvt)
            { std::string toret = str; return procStr( toret, c, cnvt ); }
        static bool esObjetoInventario(TDS * tds, Objeto * obj);
        static Plugin * buscaPlugin(const std::string &);
        static void iniciaListaPlugins();
        static void insertaEnListaPlugins(Plugin *);

        static ListaPlugins listaPlugins;

        virtual ~Plugin() = 0;

        const std::string &getNombre() const
            { return nombrePlugin; }
        const std::string &getAutor() const
            { return autorPlugin; }
        const std::string &getVersion() const
            { return versionPlugin; }
        const std::string &getHelp() const
            { return helpPlugin; }

        std::string getInfo() const;

        Lengua getLengua() const
            { return lengua; }

        void ponLengua(Lengua l)
            { lengua = l; }

        int getMaxChars() const
            { return maxChars; }

        void ponMaxChars(int chars)
            { maxChars = chars; }

        // Proceso principal
        virtual bool antesDeProcesar()
            { return true; }
        virtual void procesar(TDS *)                    = 0;
        virtual void despuesDeProcesar()
            {}

        // Proceso por localidad
        virtual bool antesDeProcesarLoc(Localidad *)
            { return true; }
        virtual void procesarLoc(Localidad *)
            {}
        virtual void despuesDeProcesarLoc(Localidad *)
            {}
};

class VoidPlugin : public Plugin {
protected:
        VoidPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        static const std::string Nombre;
        void procesar(TDS *);

        static VoidPlugin *creaVoidPlugin();
};

class ListaPlugin : public Plugin {
protected:
        ListaPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        void procesar(TDS *);

        static ListaPlugin *creaListaPlugin();

};

class HtmlPlugin : public Plugin {
protected:
        HtmlPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        void procesar(TDS *);
        std::string &cnvtStrHtml(std::string &);
        std::string cnvtStrHtml(const std::string &);

        static HtmlPlugin *creaHtmlPlugin();

};

class SrcPlugin : public Plugin {
protected:
        SrcPlugin(const std::string &, const std::string &, const std::string &, const std::string &);
        static void procesaLocalidad(FILE * f, TDS * tds, Localidad * loc);
public:
        void procesar(TDS *);

        static SrcPlugin *creaSrcPlugin();

};

class InfPlugin : public Plugin {
protected:
        class Recurso {
        public:
            static const std::string EtqGrf;
            static const std::string EtqMsc;
            static const std::string EtqPictureMark;
            static const std::string EtqMusicMark;
            static const std::string EtqCodeMark;

            std::string idLoc;
            std::string archivoRecurso;
            enum TipoRecurso { Grf, Msc };
            TipoRecurso tipoRecurso;

            const std::string &getEtq() const
                { if ( tipoRecurso == Grf ) return EtqGrf; else return EtqMsc; }
            const std::string &getMark() const
                { if ( tipoRecurso == Grf ) return EtqPictureMark; else return EtqMusicMark; }
            std::string getIdRecurso() const
                { return  getEtq() + idLoc; }
        };

        /// @brief Este es un plugin para todos los inform, parametrizable
        /// @param grf Soporta multimedia
        /// @param infate Es informATE!
        /// @param i18n Soporta distintas lenguas
        InfPlugin(const std::string &, const std::string &,
                  const std::string &, const std::string &,
                  bool grf= false, bool infate = false, bool i18n = false);

        const std::string *ids;
        bool conGraficos;
        bool _esInformATE;
        bool _esI18n;
        const std::string * jugador;
        std::string icl;
        std::string nombreArchivo;
        std::string classRoom;
        std::string classThing;
        std::string classSceneryThing;
        std::string fnFinal;

        void generarArchivoRes(const std::string &nf);
        std::string getNombreArchivo(const std::string &nf);

        typedef std::vector<Recurso> ListaRecursos;
        ListaRecursos recursos;
public:
	static const std::string Jugador;
	static const std::string ResExt;
	static const std::string UlxExt;
    static const std::string EtqThing;
    static const std::string EtqSceneryThing;
    static const std::string EtqRoom;
    static const std::string ExtInf;

    typedef enum
      { includes = 12, includesGraf, init, local, desc, luz, name,
        escenario, femenino, plural, ponible }
    TipoId;
    static const std::string strIdsInform[];
    static const std::string strIdsInformate[];
    static const std::string strIdsI6i18n[];

    void procesar(TDS *);

    bool esInformATE() const
            { return _esInformATE; }
    bool esI18n() const
            { return _esI18n; }
    bool esGrafico() const
            { return conGraficos; }

    static std::string getNombreMusica(Localidad *);
    static std::string getNombreGrafico(Localidad *);
};

class GlulxI6i18nPlugin : public InfPlugin {
protected:
        GlulxI6i18nPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        static GlulxI6i18nPlugin *creaGlulxI6i18nPlugin();

        void procesar(TDS *);
};

class GlulxInformPlugin : public InfPlugin {
protected:
        GlulxInformPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        static GlulxInformPlugin *creaGlulxInformPlugin();
};

class GlulxInformatePlugin : public InfPlugin {
protected:
        GlulxInformatePlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        static GlulxInformatePlugin *creaGlulxInformatePlugin();
};

class InformPlugin : public InfPlugin {
protected:
    InformPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
    static const std::string FinalCorrecto;

    static InformPlugin *creaInformPlugin();

};

class I6i18nPlugin : public InfPlugin {
protected:
        I6i18nPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
	static const std::string Jugador;
    static I6i18nPlugin *creaI6i18nPlugin();

    void procesar(TDS *);
};

class InformatePlugin : public InfPlugin {
protected:
        InformatePlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        static const std::string EtqLocalidad;
        static const std::string EtqObjeto;
        static const std::string EtqObjetoEstatico;
        static const std::string Jugador;
        static const std::string FinalCorrecto;
        static InformatePlugin *creaInformatePlugin();
};

class SuperglusPlugin : public Plugin {
protected:
        SuperglusPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        void procesar(TDS *);

        static SuperglusPlugin *creaSuperglusPlugin();

};

class PawsPlugin : public Plugin {
protected:
        PawsPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        void procesar(TDS *);

        static PawsPlugin *creaPawsPlugin();

};

class AGEPlugin : public Plugin, Persistente {
protected:
        AGEPlugin(const std::string &, const std::string &, const std::string &, const std::string &);

public:
        void procesar(TDS *);

        Persistente * recuperar(FILE *) { return NULL; }
        void guardar(FILE *) {}

        static AGEPlugin *creaAGEPlugin();

};

class KenshiraPlugin : public Plugin {
protected:
        KenshiraPlugin(const std::string &, const std::string &,
                       const std::string &, const std::string &);
        static const std::string Salidas[];
public:
        void procesar(TDS *);
        static KenshiraPlugin *creaKenshiraPlugin();
};

class I7Plugin : public Plugin {
protected:
        bool _esI7Sp;
        std::vector<Localidad *> locsVisitadas;
        I7Plugin(const std::string &, const std::string &,
                 const std::string &, const std::string &, bool lenguajeSp);
        static const std::string Salidas[];
        void describirLocalidad( FILE * f, Localidad *loc, TDS * tds, Localidad * from, Localidad::Direccion dir);
        void describirObjeto( TDS *, FILE * f, Objeto * obj, bool = false);
        void describirObjetosInventario(FILE * f, TDS * tds);
public:
        enum Idioma { GB, SP };
        bool esI7Sp() const
            { return _esI7Sp; }
        void procesar(TDS *);
};

class I7SpPlugin : public I7Plugin {
protected:
        I7SpPlugin(const std::string &s1, const std::string &s2,
                   const std::string &s3, const std::string &s4)
                   : I7Plugin( s1, s2, s3, s4, SP )
        {}
public:
        static I7SpPlugin *creaI7SpPlugin();
};

class I7GbPlugin : public I7Plugin {
protected:
        I7GbPlugin(const std::string &s1, const std::string &s2,
                   const std::string &s3, const std::string &s4)
                   : I7Plugin( s1, s2, s3, s4, GB )
        {}
public:
        static I7GbPlugin *creaI7GbPlugin();
};

class BasPlugin : public Plugin {
protected:
        BasPlugin(const std::string &, const std::string &,
                  const std::string &, const std::string &);
public:
        static const size_t NumLineaComienzo = 5000;
        static const std::string strDireccion[];
        void procesar(TDS *);
        static BasPlugin *creaBasPlugin();
};

class StdCPlugin : public Plugin {
protected:
        StdCPlugin(const std::string &, const std::string &,
                   const std::string &, const std::string &);
public:
        static const std::string strDireccionEN[];
        static const std::string strDireccionES[];
        static const std::string * strDireccion;
        void procesar(TDS *);
        static StdCPlugin *creaStdCPlugin();
};

class FiJsPlugin : public Plugin {
protected:
        FiJsPlugin(const std::string &, const std::string &,
                   const std::string &, const std::string &);
public:
        void procesar(TDS *);
        static FiJsPlugin *creaFiJsPlugin();
};


#endif
