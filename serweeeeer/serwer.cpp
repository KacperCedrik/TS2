//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

// Funkcje konieczne do zamiany nagl�wk�w.
std::vector<char> headerChanger(std::vector<bool> tab);
std::vector<bool> headerBinChanger(std::vector<char> tab);
std::vector<bool> binary(std::vector<char> value);
std::vector<char> decimal(std::vector<bool> value1);

// Wypisywanie wektora.
template <typename T>
void wypisz(std::vector<T> tab);

// Funkcje wykorzystywane do zamiany liczb.
std::vector<bool> dec_to_bin(int liczba, int wielkosc);
int bin_to_dec(std::vector<bool> tab);


// Operacje wykonywane na argumentach, ktorych wyniki i ID przesylane sa w naglowku.
int add(int a, int b, int c);
int subtract(int a, int b, int c);
int divide(int a, int b, int c);
int multiply(int a, int b, int c);
int whichMax(int a, int b, int c);
int whichMin(int a, int b, int c);
int multiply(int a, int b, int c);
int silnia(int arg);
bool isEqual(int a, int b, int c);


// Zmienna odpowiedzialna za numerowanie klientow i nadawanie ID.
int id_int = 1;

int main() {

    int n = 0;
    std::cout << "Na ilu maksymalnych klientow chcesz pozwolic?" << std::endl << "> ";
    std::cin >> n;

    WSAData wsaData; // Zmienna potrzebna do otworzenia socketa.
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaData) != 0) {

        // Warunek pozwalaj�cy na podstawow� obs�ug� b��d�w przy starcie winsocka.
        MessageBoxA(NULL, "Start Winsocka zostal przerwany.", "Blad", MB_OK | MB_ICONERROR);

        // Wy�wietla okienko dialogowe.
        exit(1);
    }

    // Struktura odpowiedzialna za adresacj� IP.
    SOCKADDR_IN addr;
    int addrlen = sizeof(addr);
    // W przypadku ��czenia dw�ch komputer�w, tutaj wpisujemy sw�j w�asny adres jako serwer.
    // Na komputerze klienckim wpisujemy adres serwera.
    // W przypadku korzystania z protoko�u na komputerze lokalnym u�ywamy adresu loopback: 127.0.0.1 na obu wersjach aplikacji.
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    // Socket odpowiedzialny za nas�uchiwanie.
    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);

    // Przypisujemy adres do socketa odpowiedzialnego za nas�uchiwanie.
    bind(sListen, (SOCKADDR*)&addr, sizeof(addr));

    // Funkcja odpowiedzialna za nas�uchiwanie.
    listen(sListen, SOMAXCONN);

    std::cout << "Serwer wlaczony. Oczekuje na polaczenia.  \nAby zamknac, wyslij kombinacje klawiszy CTRL+C." << std::endl << std::endl;

    // Socket odpowiedzialny za po��czenie z klientem.
    SOCKET newConnection;


    for (int i = 0; i < n; i++) {

        newConnection = accept(sListen, (SOCKADDR*)&addr, &addrlen);

        // Zmienna odpowiedzialna za numerowanie klient�w.
        int clientNum;


        if (newConnection == 0) {
            std::cout << "Wystapil blad zwiazany z brakiem polaczenia pomiedzy serwerem a klientem.";
        }
        else {

            // Numerowanie klienta.
            clientNum = id_int;
            std::cout << std::endl << "Klient " << clientNum << " podlaczony." << std::endl;

            std::vector<bool> header;

            // Pole operacji 
            std::vector<bool> operacje = { 0,0,0,0 };

            std::vector<bool> status = { 0,0,0 };

            // Liczba odpowiadaj�ca AKTUALNEJ d�ugo�ci danych, kt�r� umieszczany w nag��wku. D�ugo�� ta mo�e ulec zmianie.
            int dataLength = 6 + 64 + 3;

            // Zamiana d�ugo�ci pola danych z liczby dziesi�tnej na binarn� z pomoc� tablicy.
            std::vector<bool> vecDataLength = dec_to_bin(dataLength, 64);
            // Pole ID.
            // Zamiana ID zapisanego w postaci dziesi�tnej na binarn� z pomoc� tablicy.
            std::vector<bool> ID = dec_to_bin(id_int, 6);

            // Pole d�ugo�� nag��wka.
            // Ca�kowita d�ugo�� nag��wka. Pocz�tkowy nag��wek, b�dzie mia� d�ugo�� 112 bit�w, czyli 14 bajt�w.
            int headerLength = 148; //112
            std::vector<bool> headerLengthVec = dec_to_bin(headerLength, 64);

            // Pole wielko�� dope�nienia.
            // Okre�la wielko�� dope�nienia do wielokrotno�ci liczby 8. Zapisywane na trzech bitach.
            int fill = 0;
            std::vector<bool> fillVec = dec_to_bin(fill, 3);

            //pole sumy kontrolnej
            int sumcontrol = 1;

            // Wype�nienie pola operacji nag��wka zerami - klient jeszcze nie podj�� decyzji
            // kt�r� z 8 operacji b�dzie chcia� wykona�.
            for (int i = 0; i < 4; i++) {
                header.push_back(0);
            }

            // Wype�nienie pola statusu nag��wka.
            for (int i = 0; i < 3; i++) {
                header.push_back(status[i]);
            }

            // Wype�nienie pola d�ugo�� danych nag��wka.
            for (int i = 0; i < 64; i++) { //32
                header.push_back(vecDataLength[i]);
            }

            // Wype�nienie pola ID nag��wka.
            for (int i = 0; i < 6; i++) {
                header.push_back(ID[i]);
            }

            // Wype�nienie pola d�ugo�� nag��wka.
            for (int i = 0; i < 64; i++) {
                header.push_back(headerLengthVec[i]);
            }

        
            for (int i = 0; i < header.size(); i++) {
                if (header[i] == 1) {
                    sumcontrol++;
                }
                else {
                    sumcontrol = sumcontrol;
                }
            }

            std::vector<bool> sumcontrolVec = dec_to_bin(sumcontrol, 8);
            // Wype�nienie pola sumy kontrolnej.
            for (int i = 0; i < 8; i++) {
                header.push_back(sumcontrolVec[i]);
            }

                // Wype�nienie pola d�ugo�� dope�nienia.
            for (int i = 0; i < 3; i++) {
                header.push_back(fillVec[i]);
            }

            // Ustawienie statusu na 1 - Wysy�any zostaje header z wype�nionymi polami ID oraz status.
            status[0] = 0;
            status[1] = 0;
            status[2] = 1;

            // Wype�nienie pola status nag��wka.
            for (int i = 4; i < 7; i++) {
                header[i] = status[i - 4];
            }
            // Konwersja vector<bool> -> vector<char> z pomoc� funkcji.
            std::vector<char> headerCharToSend = decimal(header);

            // Przes�anie klientowi faktycznego nag��wka.
            send(newConnection, &headerCharToSend[0], headerCharToSend.size(), NULL);

            /*
            Operacje wykonywane po stronie klienta...
            */
            std::cout << "Odbieram dane..." << std::endl;

            // Odbieranie nag��wka z wpisan� now� d�ugo�ci� nag��wka.
            std::vector<char> headerCharReceived(19);
            recv(newConnection, &headerCharReceived[0], headerCharReceived.size(), NULL);
            std::vector<bool> headerBinRecv = binary(headerCharReceived);

            // Wy�uskanie warto�ci d�ugo�ci pola danych.
            for (int i = 0; i < 64; i++) {
                vecDataLength[i] = headerBinRecv[i + 7];
            }
            dataLength = bin_to_dec(vecDataLength);
            for (int i = 0; i < 64; i++) {
                headerLengthVec[i] = headerBinRecv[i + 77];
            }
            headerLength = bin_to_dec(headerLengthVec);
            for (int i = 0; i < 3; i++) {
                fillVec[i] = headerBinRecv[i + 141];
            }
            fill = bin_to_dec(fillVec);
            // Modyfikacja d�ugo�ci nag��wka na rzecz odebrania pe�nych danych od klienta.
            std::vector<char> headerArgRcv(headerLength / 8);
            recv(newConnection, &headerArgRcv[0], headerArgRcv.size(), NULL);
            headerBinRecv = binary(headerArgRcv);
            headerBinRecv.resize(headerLength - fill);
            std::cout << std::endl;
            headerLength -= fill;
            dataLength -= fill;
            // Modyfikacja pola status. Status 4 - odebrano dane od klienta.
            status[0] = 1;
            status[1] = 0;
            status[2] = 0;

            // Uzupe�nienie p�l nag��wka: status oraz d�ugo��.
            for (int i = 4; i < 7; i++) {
                headerBinRecv[i] = status[i - 4];
            }
            // Modyfikacja lokalnej warto�ci zmiennej d�ugo�� pola danych. Zmiana o sum� rozmiar�w trzech argument�w.
            int wielkoscArgumentu = (dataLength - 81) / 3;

            // Modyfikacja lokalnej tablicy odpowiedzialnej za reprezentacj� pola operacji.
            operacje[0] = headerBinRecv[0];
            operacje[1] = headerBinRecv[1];
            operacje[2] = headerBinRecv[2];
            operacje[3] = headerBinRecv[3];

            // Zamiana argument�w z nag��wka, na liczby dziesi�tne.
            std::vector<bool> argX;
            std::vector<bool> argY;
            std::vector<bool> argZ;

            for (int i = 0; i < wielkoscArgumentu; i++) {
                argX.push_back(headerBinRecv[i + 144]); //+8
            }
            for (int i = 0; i < wielkoscArgumentu; i++) {
                argY.push_back(headerBinRecv[i + 144 + wielkoscArgumentu]); //+8
            }
            for (int i = 0; i < wielkoscArgumentu; i++) {
                argZ.push_back(headerBinRecv[i + 144 + wielkoscArgumentu + wielkoscArgumentu]); //+8
            }

            int argX_int = bin_to_dec(argX);
            int argY_int = bin_to_dec(argY);
            int argZ_int = bin_to_dec(argZ);
            std::cout << "Pierwsza liczba: " << argX_int << std::endl
                << "Druga liczba: " << argY_int << std::endl
                << "Trzecia liczba: " << argZ_int << std::endl;
            // Tablica w kt�rej przechowywany jest wynik. Jej wielko�� ustawiany jest na potrojon� warto�� rozmiaru argument�w.
            int result = wielkoscArgumentu * 3;
            std::vector<bool> vecResult(wielkoscArgumentu * 3);

            // W zale�no�ci od podanej operacji wykonujemy zestaw okre�lonych funkcji.
            // Jedyn� r�nic� jest operacja odejmowania, w kt�rej ostatni bit liczby ustalamy jako kod znaku.
            if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 0 && operacje[3] == 0) {
                std::cout << "Wynik potegowania: " << multiply(argX_int, argY_int, argZ_int) << std::endl;
                if (multiply(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(add(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 0 && operacje[3] == 1) {
                std::cout << "Wynik dzielenia: " << divide(argX_int, argY_int, argZ_int) << std::endl;
                if (divide(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(divide(argX_int, argY_int, argZ_int), vecResult.size());
                }
                if (divide(argX_int, argY_int, argZ_int) < 0) vecResult[0] = 1;
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 1 && operacje[3] == 0) {
                std::cout << "Wynik mnozenie: " << multiply(argX_int, argY_int, argZ_int) << std::endl;
                if (multiply(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(multiply(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 0 && operacje[2] == 1 && operacje[3] == 1) {
                std::cout << "Wynik odejmowania: " << subtract(argX_int, argY_int, argZ_int) << std::endl;
                if (subtract(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(subtract(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 0 && operacje[2] == 0) {
                std::cout << "Ktora wieksza: " << whichMax(argX_int, argY_int, argZ_int) << std::endl;
                if (whichMax(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(whichMax(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 0 && operacje[2] == 1) {
                std::cout << "Ktora mniejsza: " << whichMin(argX_int, argY_int, argZ_int) << std::endl;
                if (whichMin(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(whichMin(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 1 && operacje[3] == 0) {
                std::cout << "Czy liczby sa rowne? " << isEqual(argX_int, argY_int, argZ_int) << std::endl;
                if (isEqual(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(isEqual(argX_int, argY_int, argZ_int), vecResult.size());
                }
            }
            else if (operacje[0] == 0 && operacje[1] == 1 && operacje[2] == 1 && operacje[2] == 1) {
                std::cout << "Wynik dodawania: " << add(argX_int, argY_int, argZ_int) << std::endl;
                if (add(argX_int, argY_int, argZ_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                vecResult = dec_to_bin(add(argX_int, argY_int, argZ_int), vecResult.size());
            }
            else if (operacje[0] == 1 && operacje[1] == 1 && operacje[2] == 1 && operacje[2] == 1) {
                std::cout << "Wynik silni: " << silnia(argX_int) << std::endl;
                if (silnia(argX_int) > pow(2, result) - 1) {
                    std::cout << "Wartosc wyniku wykracza poza rozmiar";
                    vecResult = dec_to_bin(0, vecResult.size());
                }
                else {
                    vecResult = dec_to_bin(silnia(argX_int), vecResult.size());
                }
            }
            



            // Modyfikacja lokalnego pola d�ugo�� pola danych.
            dataLength += wielkoscArgumentu * 3;
            headerLength += wielkoscArgumentu * 3;
            std::vector<char> headerCharToSendResult = decimal(headerBinRecv);

            std::cout << std::endl;
            // Zwiekszenie liczby odpowiadaj�cej d�ugo�ci pola danych.
            //dataLength += 10;
            //headerLength += 10;
            fill = 8 - (headerLength % 8);
            headerLength += fill;
            dataLength += fill;
            vecDataLength = dec_to_bin(dataLength, 64); //32
            fillVec = dec_to_bin(fill, 3);
            for (int i = 7; i < 71; i++) {
                headerBinRecv[i] = vecDataLength[i - 7];
            }
            for (int i = 77; i < 141; i++) {
                headerBinRecv[i] = headerLengthVec[i - 77];
            }
            for (int i = 141; i < 144; i++) {
                headerBinRecv[i] = fillVec[i - 141];
            }
            // Uzupe�nienie nag��wka odpowiednimi danymi.
            for (int i = 7; i < 71; i++) {
                headerBinRecv[i] = vecDataLength[i - 7];
            }
            for (int i = 0; i < vecResult.size(); i++) {
                headerBinRecv.push_back(vecResult[i]);
            }

            int sumcontrol2 = 0;
            for (int i = 0; i < headerBinRecv.size(); i++) {
                if (headerBinRecv[i] == 1) {
                    sumcontrol2++;
                }
                else {
                    sumcontrol2 = sumcontrol2;
                }
            }
            // Ustawienie statusu 5 - wys�anie ostatecznych danych.
            status[0] = 1;
            status[1] = 0;
            status[2] = 1;

            for (int i = 4; i < 7; i++) {
                headerBinRecv[i] = status[i - 4];
            }
            std::vector<bool> sumcontrolVec2 = dec_to_bin(sumcontrol2, 8);
            for (int i = 0; i < 7; i++) {
                headerBinRecv.push_back(sumcontrolVec2[i]);
            }



            // Przes�anie ostatecznych danych wynikowych w postaci nag��wka (w postaci tablicy char utworzonej z tablicy bool).
            headerCharToSendResult = decimal(headerBinRecv);
            send(newConnection, &headerCharToSendResult[0], headerCharToSendResult.size(), NULL);
            std::cout << std::endl;
            // Zwi�kszenie liczby ID klienta dla potencjalnego nast�pnego klienta.
            id_int++;

            std::cout << "Zamykanie polaczenia z klientem: " << clientNum << std::endl << std::endl;
            closesocket(newConnection);
        }
    }
    return 0;
}

std::vector<char> headerChanger(std::vector<bool> tab) {
    std::vector<char> header(tab.size());
    for (int i = 0; i < tab.size(); i++) {
        if (tab[i] == 0) {
            header[i] = '0';
        }
        else if (tab[i] == 1) {
            header[i] = '1';
        }
    }
    return header;
}

std::vector<bool> headerBinChanger(std::vector<char> tab) {
    std::vector<bool> header(tab.size());
    for (int i = 0; i < tab.size(); i++) {
        if (tab[i] == '0') {
            header[i] = 0;
        }
        else if (tab[i] == '1') {
            header[i] = 1;
        }
    }
    return header;
}

std::vector<bool> dec_to_bin(int liczba, int wielkosc)
{
    std::vector<bool> tab(wielkosc);


    int i = wielkosc - 1;
    while (liczba) //dop�ki liczba b�dzie r�na od zera
    {
        tab[i--] = liczba % 2;
        liczba /= 2;
    }
    return tab;
}


int bin_to_dec(std::vector<bool> tab) {
    int wielkosc = tab.size();
    int dec = 0;
    for (int i = 0; i < wielkosc; i++) {
        int pom = tab[i] * pow(2, wielkosc - i - 1);
        dec += pom;
    }
    return dec;
}
// funkcje matematyczne
int add(int a, int b, int c) {
    return a + b + c;
}
int subtract(int a, int b, int c) {
    return a - b - c;
}
int divide(int a, int b, int c) {
    return a / (b * c);
}
int multiply(int a, int b, int c) {
    return a * b * c;
}
int whichMax(int a, int b, int c) {
    if (a > b > c) return a;
    if (a > c > b) return a;
    if (b > a > c) return b;
    if (b > c > a) return b;
    if (c > a > b) return c;
    if (c > b > a) return c;
}
int whichMin(int a, int b, int c) {
    if (a > b > c) return c;
    if (a > c > b) return b;
    if (b > a > c) return c;
    if (b > c > a) return a;
    if (c > a > b) return b;
    if (c > b > a) return a;
}
bool isEqual(int a, int b, int c) {
    if (a == b == c) return false; else return true;
}
int multiplying(int a, int b, int c) {
    return a ^ b;
}
int silnia(int arg) {
    if (arg < 2) return arg;
    return arg * silnia(arg - 1);
}

std::vector<char> decimal(std::vector<bool>  value1) {
    std::vector<char> value;
    if (value1.size() % 8 == 0) {
        value.resize(value1.size() / 8);
        for (int i = 0; i < value.size(); i++) {
            int pom2 = 8 * i;
            std::vector<bool> pom(8);
            for (int j = 0; j < 8; j++) {
                pom[j] = value1[pom2 + j];
            }
            value[i] = bin_to_dec(pom);
        }
    }
    else {
        int pom = value1.size() % 8;
        int dopelnienie = 8 - pom;
        value.resize((value1.size() + dopelnienie) / 8);
        for (int i = 0; i < value.size(); i++) {
            int pom2 = 8 * i;
            std::vector<bool> vecpom(8);
            for (int j = 0; j < 8; j++) {
                if (pom2 + j >= value1.size()) {
                    vecpom[j] = 0;
                }
                else vecpom[j] = value1[pom2 + j];
            }
            value[i] = bin_to_dec(vecpom);
        }
    }
    return value;
}

std::vector<bool> binary(std::vector<char> value) {
    std::vector<bool> header;
    std::vector<bool> pom(8);
    for (int i = 0; i < value.size(); i++) {
        unsigned char k = value[i];
        int test = k;
        pom = dec_to_bin(test, 8);
        for (int j = 0; j < pom.size(); j++) {
            header.push_back(pom[j]);
        }
    }
    return header;
}

template<typename T>
void wypisz(std::vector<T> tab) {
    for (int i = 0; i < tab.size(); i++) {
        std::cout << tab[i];
    }
}