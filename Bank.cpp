#include "sha256.h"
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <random>
#include <unordered_map>
#include <cmath>

SHA256 sha256;
std::string gen_salt(size_t length = 16){
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(33,126);

    std::string salt = "";
    for(size_t i = 0; i < length; i++){
        salt += static_cast<char>(distribution(generator));
    }
    return salt;
}

double Zins;
double start_purse;
double start_account;

class Kunde{
    protected:
        std::string name;
        double purse;
        int ID;
        double val;
    public:
        std::string salt;
        std::string password_hash;
        std::chrono::time_point<std::chrono::system_clock> last_login;

        Kunde() : name(""), purse(0), ID(0), val(0), salt(""), password_hash(""), last_login(std::chrono::system_clock::now()) {}
        Kunde(std::string K_name, double K_purse, int K_ID, double K_val, std::string K_salt, std::string K_passsword_hash, std::chrono::time_point<std::chrono::system_clock> K_last_login) : 
        name(K_name), purse(K_purse), ID(K_ID), val(K_val), salt(K_salt), password_hash(K_passsword_hash), last_login(K_last_login){}
        
        std::string getName() const {return name;}
        double getPurse() const {return purse;}
        int getID() const {return ID;}
        double getVal() const {return val;}

        void Bargeld() const{
            std::cout<<"In deinem Portmonaie befinden sich "<<purse<<"$."<<std::endl;
        }

        void Kontostand() const{
            std::cout<<"Auf deinem Konto befinden sich "<<val<<"$."<<std::endl;
        }

        void Geld_abheben(const double& Geld){
            if (Geld<=val){
                val-=Geld;
                purse+=Geld;
                std::cout<<"Du hast "<<Geld<<"$ abgehoben."<<std::endl;
            }
            else{
                std::cout<<"Du hast nicht genug Geld auf dem Konto und hebst deswegen den Maximalbetrag ab."<<std::endl;
                purse+=val;
                std::cout<<"Du hast "<<purse<<"$ abgehoben."<<std::endl;
                val=0;
            }
            Bargeld();
            Kontostand();
            std::cout<<std::endl;
        }

        void Geld_einzahlen(const double& Geld){
            if (Geld<=purse){
                purse-=Geld;
                val +=Geld;
                std::cout<<"Du hast "<<Geld<<"$ eingezahlt."<<std::endl;
            }
            else{
                std::cout<<"Du besitzt nicht genung Bargeld und zahlst deswegen alles ein."<<std::endl;
                val+=purse;
                std::cout<<"Du hast "<<purse<<"$ eingezahlt."<<std::endl;
                purse=0;
            }
            Bargeld();
            Kontostand();
            std::cout<<std::endl;
        }

        void überweisen(std::unordered_map<std::string,Kunde>& ks, const Kunde& empfänger, const double& Geld){
            if (Geld<=val){
                double em_val = empfänger.getVal();
                em_val += Geld;
                val-=Geld;
                std::string em_name = empfänger.getName() + "_" + std::to_string(empfänger.getID());
                Kunde k(empfänger.getName(), empfänger.getPurse(), empfänger.getID(), em_val, empfänger.salt, empfänger.password_hash, empfänger.last_login);
                ks[em_name]=k;
                std::cout<<"Du hast dem User "<<em_name<<" "<<Geld<<"$ überwiesen."<<std::endl;
            }
            else{
                std::cout<<"Du hast nicht genug Geld weswegen der Maximalbetrag überwiesen wird."<<std::endl;
                double em_val = empfänger.getVal();
                em_val += val;
                std::string em_name = empfänger.getName() + "_" + std::to_string(empfänger.getID());
                Kunde k(empfänger.getName(), empfänger.getPurse(), empfänger.getID(), em_val, empfänger.salt, empfänger.password_hash, empfänger.last_login);
                ks[em_name]=k;
                std::cout<<"Du hast dem User "<<em_name<<" "<<val<<"$ überwiesen."<<std::endl;  
                val = 0;
            }
        }

        void Zinsen(){
            auto duration0 = last_login.time_since_epoch();
            int t0 = std::chrono::duration_cast<std::chrono::hours>(duration0).count();
            auto time = std::chrono::system_clock::now();
            auto duration1 = time.time_since_epoch();
            int t1 = std::chrono::duration_cast<std::chrono::hours>(duration1).count();
            int t = t1-t0;
            double alt_val = val;
            val = val*std::pow(Zins,t);
            std::cout<<"Du hast "<<(val-alt_val)<<"$ Zinsen erhalten."<<std::endl;
        }

        virtual ~Kunde(){}
};

class Admin : public Kunde{
    public:
        Admin() : Kunde() {}
        Admin(std::string A_name,double A_purse,int A_ID,double A_val,std::string A_salt,std::string A_hash,std::chrono::time_point<std::chrono::system_clock> A_login)
        : Kunde(A_name,A_purse,A_ID,A_val,A_salt,A_hash,A_login){}

        void edit_value(double Geld){
            val=Geld;
            std::cout<<"Dein Kontostand wurde auf "<<val<<"$ gesetzt."<<std::endl;
        }

        void Kunden_liste(const std::unordered_map<std::string, Kunde>& ks){
            std::cout<<"Kunden liste wird ausgegeben."<<std::endl;
            for (const auto& [key,value]:ks){
                std::cout<<"Name_ID: "<<value.getName()<<"_"<<value.getID()<<" Purse: "<<value.getPurse()<<"$ Value: "<<value.getVal()<<"$ \n";
            }
            std::cout<<"Kunden liste wurde ausgegeben."<<std::endl;
        }

        void edit_zins(double zinssatz){
            Zins = zinssatz;
            std::cout<<"Der Zinssatz wurde auf "<<Zins<<" gesetzt."<<std::endl;
        }
};

enum class Kontoart{
    Standard = 1,
    Admin
};

enum class Kontofunktionen{
    Bargeld = 1,
    Kontostand,
    Geld_abheben,
    Geld_einzahlen,
    Überweisen,
    edit_val,
    Kunden_liste,
    edit_zins
};

int last_ID=0;
std::unordered_map<std::string, Kunde> Kunden;
std::unordered_map<std::string, Admin> Admins;
std::string Admin_salt = "f=w@;zLL!0cdz}t>";
std::string Admin_hash = "ce3ca1b9a01343676efa74af881d940d8327dd9589e4e2f8410fa03a29a495e0";

template <typename T>
T User_Input(const std::string& question){
    std::string ui;
    T result;
    while (true){
        std::cout<<question;
        std::cin>>ui;
        std::cout<<std::endl;

        std::stringstream UI(ui);
        if (!(UI >> result)){
            std::cout<<"Falsche eingabe probiere es erneut mit dem richtigen Datentyp."<<std::endl;
            continue;
        }
        break;
    }
    return result;
}

Kontoart UI_Kontoart(){
    int user_input = User_Input<int>("Für ein Standard Konto wähle [1], für ein Admin Konto [2]. ");
    Kontoart kontoart = static_cast<Kontoart>(user_input);
        
    switch(kontoart){
        case Kontoart::Standard:
            return Kontoart::Standard;
            break;
        case Kontoart::Admin:
            return Kontoart::Admin;
            break;
        default:
            std::cout<<"Bitte tätige einge gültige Eingabe"<<std::endl;
            UI_Kontoart();
            break;
    }

    return Kontoart::Standard;
} 

void speichern(const std::unordered_map<std::string, Kunde>& ks, const std::unordered_map<std::string, Admin>& ka, const double& Zins, const double& s_purse, const double& s_account){
    std::cout<<"Daten werden gespeichert ..."<<std::endl;
    std::ofstream f1("bank_data.txt");
    for (auto const& [key,k]:ks){
        auto login_time = k.last_login;
        auto time = login_time.time_since_epoch();
        auto t = std::chrono::duration_cast<std::chrono::seconds> (time).count();
        f1<<k.getName()<<" "<<k.getPurse()<<" "<<k.getID()<<" "<<k.getVal()<<" "<<k.salt<<" "<<k.password_hash<<" "<<t<<"\n";
    }
    f1.close();

    std::cout<<"Kunden erfolgreich gespeichert."<<std::endl;
    std::ofstream f2("bank_admins.txt");
    for (auto const& [key,k]:ka){
        auto login_time = k.last_login;
        auto time = login_time.time_since_epoch();
        auto t = std::chrono::duration_cast<std::chrono::seconds> (time).count();
        f2<<k.getName()<<" "<<k.getPurse()<<" "<<k.getID()<<" "<<k.getVal()<<" "<<k.salt<<" "<<k.password_hash<<" "<<t<<"\n";
    }
    f2.close();

    std::ofstream f3("bank_settigns.txt");
    f3<<Zins<<" "<<s_purse<<" "<<s_account;
    f3.close();
    std::cout<<"Settings wurden gespeichert."<<std::endl;
    std::cout<<"Admins erfolgreich gespeichert."<<std::endl;
    std::cout<<"Alle Daten wurden gespeichert."<<std::endl;
}

void laden(std::unordered_map<std::string, Kunde>& ks, std::unordered_map<std::string, Admin>& ka, double& Zins, double& s_purse, double& s_account){
    std::cout<<"Daten werden geladen ..."<<std::endl;

    std::string name,salt,hash;
    int ID;
    double purse, val;
    long long t;

    std::ifstream f1("bank_data.txt");
    if (!f1.is_open()){return;}
    while (f1>>name>>purse>>ID>>val>>salt>>hash>>t){
        std::chrono::seconds duration(t);
        std::chrono::system_clock::time_point time(duration);
        Kunde k(name,purse,ID,val,salt,hash,time);
        std::string login_name = name + "_" + std::to_string(ID);
        ks[login_name] = k;
        if (ID > last_ID) last_ID = ID;
    }
    f1.close();

    std::cout<<"Kunden wurden geladen."<<std::endl;
    std::ifstream f2("bank_admins.txt");
    if (!f2.is_open()){return;}
    while (f2>>name>>purse>>ID>>val>>salt>>hash>>t){
        std::chrono::seconds duration(t);
        std::chrono::system_clock::time_point time(duration);
        Admin a(name,purse,ID,val,salt,hash,time);
        std::string login_name ="A_" + name + "_" + std::to_string(ID);
        ka[login_name] = a;
        if (ID > last_ID) last_ID = ID;
    }
    f2.close();

    std::cout<<"Admins wurden geladen."<<std::endl;

    std::ifstream f3("bank_settings.txt");
    if (!f3.is_open()){Zins=1.02;s_purse=100;s_account=900;return;}
    f3>>Zins>>s_purse>>s_account;
    f3.close();
    std::cout<<"Alle Daten wurden geladen."<<std::endl;
}

void Konto_erstellen(){
    std::string name;
    double purse;
    int ID;
    double val;
    std::string password_hash;
    std::string password;
    std::string salt;

    Kontoart kontoart = UI_Kontoart();

    std::cout<<"Gebe einen Namen an: ";
    std::cin>>name;
    std::cout<<std::endl;

    std::cout<<"Gebe ein Passwort an: ";
    std::cin>>password;
    std::cout<<std::endl;

    salt = gen_salt();
    password_hash = sha256(password+salt);
    password.assign(password.length(), '0');

    purse = start_purse;
    ID = last_ID+1;
    last_ID+=1;
    val = start_account;
    auto t = std::chrono::system_clock::now();
    std::string login_name;

    if (kontoart==Kontoart::Standard){
        login_name = name +"_" + std::to_string(ID);
        Kunde k(name, purse, ID, val, salt, password_hash,t);
        Kunden[login_name] = k;
    }
    else if (kontoart==Kontoart::Admin){
        std::string User_hash;
        std::string A_password;
        std::cout<<"Admin Passwort: ";
        std::cin>>A_password;
        std::cout<<std::endl;
        User_hash = sha256(A_password+Admin_salt);
        A_password.assign(A_password.length(),'0');
        if (User_hash==Admin_hash){
            login_name = "A_"+name+"_"+std::to_string(ID);
            Admin a(name,purse,ID,val,salt,password_hash,t);
            Admins[login_name] = a;
        }
    }
    std::cout<<"Dein Login Name ist: "<<login_name<<std::endl;
}

bool correct_Password(std::string& password, const std::string& username, const Kontoart& kontoart){
    std::string salt;
    std::string Kunden_hash;
    std::string User_hash;
    if (kontoart==Kontoart::Standard){
        Kunde& k = Kunden[username];
        salt = k.salt;
        Kunden_hash = k.password_hash;
    }
    else if(kontoart==Kontoart::Admin){
        Admin& a = Admins[username];
        salt = a.salt;
        Kunden_hash = a.password_hash;
    }
    User_hash = sha256(password+salt);
    password.assign(password.length(),'0');
    if (Kunden_hash==User_hash){
        return true;
    }
    else{
        return false;
    }
}

void Konto(const std::string& username,const Kontoart& kontoart){
    if (kontoart==Kontoart::Standard){
        Kunde& k = Kunden[username];
        k.Zinsen();
        k.last_login = std::chrono::system_clock::now();

        int acitivity = User_Input<int>(" Bargeld[1] \n Kontostand[2] \n Geld abheben[3] \n Geld einzahlen[4] \n Geld überweisen[5] \n zum verlassen [6] \n ");
        Kontofunktionen kontofunktionen = static_cast<Kontofunktionen>(acitivity);

        switch (kontofunktionen){
            case Kontofunktionen::Bargeld: {
                k.Bargeld();
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Kontostand: {
                k.Kontostand();
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Geld_abheben:{
                double Geld = User_Input<double>("Wie viel Geld willst du abheben ? ");
                k.Geld_abheben(Geld);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Geld_einzahlen:{
                double Geld = User_Input<double>("Wie viel Geld willst du einzahlen ? ");
                k.Geld_einzahlen(Geld);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Überweisen:{
                double Geld;
                std::string empfänger_name = User_Input<std::string>("An wen(Name_ID) möchtest du Geld überweisen ? ");
                if (Kunden.find(empfänger_name)!=Kunden.end()){
                    Kunde k_em = Kunden[empfänger_name];
                    Geld = User_Input<double>("Wie viel Geld möchtest du überweisen ? ");
                    k.überweisen(Kunden,k_em,Geld);
                }
                else{
                    std::cout<<"Bitte gib einen Gültigen Empfänger ein."<<std::endl;
                }
                Konto(username,kontoart);
                break;
            }
            default:
                return;
                break;
        }
    }
    else if (kontoart==Kontoart::Admin){
        Admin& a = Admins[username];
        a.Zinsen();
        a.last_login = std::chrono::system_clock::now();

        int acitivity = User_Input<int>(" Bargeld[1] \n Kontostand[2] \n Geld abheben[3] \n Geld einzahlen[4] \n Geld überweisen[5] \n edit value[6] \n Kunden Liste[7] \n Zinssatz ändern[8] \n ");
        Kontofunktionen kontofunktionen = static_cast<Kontofunktionen>(acitivity);

        switch (kontofunktionen){
            case Kontofunktionen::Bargeld: {
                a.Bargeld();
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Kontostand: {
                a.Kontostand();
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Geld_abheben:{
                double Geld = User_Input<double>("Wie viel Geld willst du abheben ? ");
                a.Geld_abheben(Geld);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Geld_einzahlen:{
                double Geld = User_Input<double>("Wie viel Geld willst du einzahlen ? ");
                a.Geld_einzahlen(Geld);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Überweisen:{
                double Geld;
                std::string empfänger_name = User_Input<std::string>("An welchen Kunden(Name_ID) möchtest du Geld überweisen ? ");
                if (Kunden.find(empfänger_name)!=Kunden.end()){
                    Kunde k_em = Kunden[empfänger_name];
                    Geld = User_Input<double>("Wie viel Geld möchtest du überweisen ? ");
                    a.überweisen(Kunden,k_em,Geld);
                }
                else{
                    std::cout<<"Bitte gib einen Gültigen Empfänger ein. Der Empfänger muss ein Kunde sein!"<<std::endl;
                }
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::edit_val:{
                double Geld = User_Input<double>("Wie viel Geld soll auf deinem Konto sein ? ");
                a.edit_value(Geld);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::Kunden_liste:{
                a.Kunden_liste(Kunden);
                Konto(username,kontoart);
                break;
            }
            case Kontofunktionen::edit_zins:{
                double Zinssatz = User_Input<double>("Wie hoch soll der Zinssatz sein ? ");
                a.edit_zins(Zinssatz);
                Konto(username,kontoart);
                break;
            }
            default:
                return;
                break;
        }
    }
}

void login(){
    Kontoart kontoart = UI_Kontoart();
    std::string username = User_Input<std::string>("Name_ID: ");
    std::string password;

    switch(kontoart){
        case Kontoart::Standard:{
            if(Kunden.find(username) != Kunden.end()){
                password = User_Input<std::string>("Password: ");
                if(correct_Password(password,username,kontoart)){
                    Konto(username,kontoart);
                }
                else{
                    std::cout<<"Das Passwort ist falsch"<<std::endl;
                    }
                password.assign(password.length(),'0');
            }
            else{
                std::cout<<"Name_ID existiert nicht"<<std::endl;
            }
            break;
        }
        case Kontoart::Admin:{
            if(Admins.find(username) != Admins.end()){
                password = User_Input<std::string>("Password: ");
                if(correct_Password(password,username,kontoart)){
                    Konto(username,kontoart);
                }
                else{
                    std::cout<<"Das Passwort ist falsch"<<std::endl;
                }
                password.assign(password.length(),'0');
            }
            else{
                std::cout<<"Name_ID existiert nicht"<<std::endl;
            }
            break;
        }
        default:{
            break;
        }
    }
}

int main(){
    SetConsoleOutputCP(CP_UTF8);
    laden(Kunden,Admins,Zins,start_purse,start_account);
    std::string programm;
    while(true){
        programm = User_Input<std::string>("Möchtest du dich einloggen[1], ein neues Konto erstellen[2] oder die Simulation verlassen[q] ? ");
        if (programm=="1"){
            login();
        }
        else if (programm=="2"){
            Konto_erstellen();
        }
        else if (programm=="q"){
            break;
        }
        else{
            std::cout<<"Keine gültige eingabe, Versuch es nochmal oder drücke [q] zum verlassen."<<std::endl;
        }
    }
    speichern(Kunden,Admins,Zins,start_purse,start_account);
    return 0;
}