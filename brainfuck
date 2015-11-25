// Spocita modulo
int modulo(int n, int b){
    int h = n / b;
    h = h * b;
    h = n - h;
    return h;
}

// Zakoduje cislo od 0 do 9 do podoby stringu
string encode(int n){
    if (n == 0){
        return "0";
    }
    else{
    if (n == 1){
        return "1";
    }
    else{
    if (n == 2){
        return "2";
    }
    else{
    if (n == 3){
        return "3";
    }
    else{
    if (n == 4){
        return "4";
    }
    else{
    if (n == 5){
        return "5";
    }
    else{
    if (n == 6){
        return "6";
    }
    else{
    if (n == 7){
        return "7";
    }
    else{
    if (n == 8){
        return "8";
    }
    else{}}}}}}}}}
    return "9";
}

// Zakoduje nezaporne cele cislo do stringu
// Cislo po zakodovani je vzdy dlouhe tri znaky
// (pokud obsahuje mene nez tri cislice, jsou pridany na zacatek nuly)
string toStr(int val){
    int d = val;
    string res = "";
    string c = "";
    int h;
    for (int i = 0; i < 3; i = i + 1){
        h = modulo(d,10);
        d = d / 10;
        c = encode(h);
        res = concat(c,res);
    }
    return res;
}

// Zkonvertuje jednu cislici obsazenou ve stringu do integeru
int decode(string s){
    if (s=="0"){
        return 0;
    }
    else {
    if (s=="1"){
        return 1;
    }
    else {
    if (s=="2"){
        return 2;
    }
    else {
    if (s=="3"){
        return 3;
    }
    else {
    if (s=="4"){
        return 4;
    }
    else {
    if (s=="5"){
        return 5;
    }
    else {
    if (s=="6"){
        return 6;
    }
    else {
    if (s=="7"){
        return 7;
    }
    else {
    if (s=="8"){
        return 8;
    }
    else {
    }}}}}}}}}
    return 9;
}

// Zkonvertuje cislo ve stringu do integeru
int toInt(string s){
    int res = 0;
    int pow = 1;
    int len;
    len = length(s);
    for (int i = len - 1; i >= 0; i = i - 1){
        string s1;
        s1 = substr(s, i, 1);
        int d;
        d = decode(s1);
        res = res + d * pow;
        pow = pow * 10;
    }
    return res;
}

// Upravi hodnotu v pameti data na indexu p
// Pokud je isInc nulove, hodnota je dekrementovana, v opacnem pripade je inkrementovana
// Hodnoty v pameti mohou byt pouze od 0 do 255, 255 + 1 = 0, 0 - 1 = 255
string write(string data, int p, int isInc){
    // nalezneme upravovanou hodnotu
	auto p4 = p * 4;
	string tmp;
	tmp = substr(data, p4, 3);

    // zkonvertujeme na int
	int val;
	val = toInt(tmp);
	if(isInc){
        // inkrementace
		val = val + 1;
		if(val > 255){
			val = 0;
		} else {}
	} else {
        // dekrementace
		val = val - 1;
		if(val < 0){
			val = 255;
		} else{}
	}

    // prevedeme zpatky na string
	tmp = toStr(val);

    // ziskame vse, co bylo na leve a prave strane od upravovane bunky
	string l;
	string r;
	l = substr(data, 0, p4);
	auto p44 = p4 + 3;
	int len;
	len = length(data);
	len = len - p44;
	r = substr(data, p44, len);

    // a spojime zpatky dohromady s novou hodnotou
	data = concat(l, tmp);
	data = concat(data, r);

	return data;
}

// vytiskne hodnotu ulozenou ve stringu v ASCII podobe
// tato funkce byla automaticky generovana skriptem v pythonu, autor neni magor
int print(string byte){
	if(byte == "000"){
        cout << "\x00";
    } else { if(byte == "001"){
        cout << "\x01";
    } else { if(byte == "002"){
        cout << "\x02";
    } else { if(byte == "003"){
        cout << "\x03";
    } else { if(byte == "004"){
        cout << "\x04";
    } else { if(byte == "005"){
        cout << "\x05";
    } else { if(byte == "006"){
        cout << "\x06";
    } else { if(byte == "007"){
        cout << "\x07";
    } else { if(byte == "008"){
        cout << "\x08";
    } else { if(byte == "009"){
        cout << "\x09";
    } else { if(byte == "010"){
        cout << "\x0a";
    } else { if(byte == "011"){
        cout << "\x0b";
    } else { if(byte == "012"){
        cout << "\x0c";
    } else { if(byte == "013"){
        cout << "\x0d";
    } else { if(byte == "014"){
        cout << "\x0e";
    } else { if(byte == "015"){
        cout << "\x0f";
    } else { if(byte == "016"){
        cout << "\x10";
    } else { if(byte == "017"){
        cout << "\x11";
    } else { if(byte == "018"){
        cout << "\x12";
    } else { if(byte == "019"){
        cout << "\x13";
    } else { if(byte == "020"){
        cout << "\x14";
    } else { if(byte == "021"){
        cout << "\x15";
    } else { if(byte == "022"){
        cout << "\x16";
    } else { if(byte == "023"){
        cout << "\x17";
    } else { if(byte == "024"){
        cout << "\x18";
    } else { if(byte == "025"){
        cout << "\x19";
    } else { if(byte == "026"){
        cout << "\x1a";
    } else { if(byte == "027"){
        cout << "\x1b";
    } else { if(byte == "028"){
        cout << "\x1c";
    } else { if(byte == "029"){
        cout << "\x1d";
    } else { if(byte == "030"){
        cout << "\x1e";
    } else { if(byte == "031"){
        cout << "\x1f";
    } else { if(byte == "032"){
        cout << "\x20";
    } else { if(byte == "033"){
        cout << "\x21";
    } else { if(byte == "034"){
        cout << "\x22";
    } else { if(byte == "035"){
        cout << "\x23";
    } else { if(byte == "036"){
        cout << "\x24";
    } else { if(byte == "037"){
        cout << "\x25";
    } else { if(byte == "038"){
        cout << "\x26";
    } else { if(byte == "039"){
        cout << "\x27";
    } else { if(byte == "040"){
        cout << "\x28";
    } else { if(byte == "041"){
        cout << "\x29";
    } else { if(byte == "042"){
        cout << "\x2a";
    } else { if(byte == "043"){
        cout << "\x2b";
    } else { if(byte == "044"){
        cout << "\x2c";
    } else { if(byte == "045"){
        cout << "\x2d";
    } else { if(byte == "046"){
        cout << "\x2e";
    } else { if(byte == "047"){
        cout << "\x2f";
    } else { if(byte == "048"){
        cout << "\x30";
    } else { if(byte == "049"){
        cout << "\x31";
    } else { if(byte == "050"){
        cout << "\x32";
    } else { if(byte == "051"){
        cout << "\x33";
    } else { if(byte == "052"){
        cout << "\x34";
    } else { if(byte == "053"){
        cout << "\x35";
    } else { if(byte == "054"){
        cout << "\x36";
    } else { if(byte == "055"){
        cout << "\x37";
    } else { if(byte == "056"){
        cout << "\x38";
    } else { if(byte == "057"){
        cout << "\x39";
    } else { if(byte == "058"){
        cout << "\x3a";
    } else { if(byte == "059"){
        cout << "\x3b";
    } else { if(byte == "060"){
        cout << "\x3c";
    } else { if(byte == "061"){
        cout << "\x3d";
    } else { if(byte == "062"){
        cout << "\x3e";
    } else { if(byte == "063"){
        cout << "\x3f";
    } else { if(byte == "064"){
        cout << "\x40";
    } else { if(byte == "065"){
        cout << "\x41";
    } else { if(byte == "066"){
        cout << "\x42";
    } else { if(byte == "067"){
        cout << "\x43";
    } else { if(byte == "068"){
        cout << "\x44";
    } else { if(byte == "069"){
        cout << "\x45";
    } else { if(byte == "070"){
        cout << "\x46";
    } else { if(byte == "071"){
        cout << "\x47";
    } else { if(byte == "072"){
        cout << "\x48";
    } else { if(byte == "073"){
        cout << "\x49";
    } else { if(byte == "074"){
        cout << "\x4a";
    } else { if(byte == "075"){
        cout << "\x4b";
    } else { if(byte == "076"){
        cout << "\x4c";
    } else { if(byte == "077"){
        cout << "\x4d";
    } else { if(byte == "078"){
        cout << "\x4e";
    } else { if(byte == "079"){
        cout << "\x4f";
    } else { if(byte == "080"){
        cout << "\x50";
    } else { if(byte == "081"){
        cout << "\x51";
    } else { if(byte == "082"){
        cout << "\x52";
    } else { if(byte == "083"){
        cout << "\x53";
    } else { if(byte == "084"){
        cout << "\x54";
    } else { if(byte == "085"){
        cout << "\x55";
    } else { if(byte == "086"){
        cout << "\x56";
    } else { if(byte == "087"){
        cout << "\x57";
    } else { if(byte == "088"){
        cout << "\x58";
    } else { if(byte == "089"){
        cout << "\x59";
    } else { if(byte == "090"){
        cout << "\x5a";
    } else { if(byte == "091"){
        cout << "\x5b";
    } else { if(byte == "092"){
        cout << "\x5c";
    } else { if(byte == "093"){
        cout << "\x5d";
    } else { if(byte == "094"){
        cout << "\x5e";
    } else { if(byte == "095"){
        cout << "\x5f";
    } else { if(byte == "096"){
        cout << "\x60";
    } else { if(byte == "097"){
        cout << "\x61";
    } else { if(byte == "098"){
        cout << "\x62";
    } else { if(byte == "099"){
        cout << "\x63";
    } else { if(byte == "100"){
        cout << "\x64";
    } else { if(byte == "101"){
        cout << "\x65";
    } else { if(byte == "102"){
        cout << "\x66";
    } else { if(byte == "103"){
        cout << "\x67";
    } else { if(byte == "104"){
        cout << "\x68";
    } else { if(byte == "105"){
        cout << "\x69";
    } else { if(byte == "106"){
        cout << "\x6a";
    } else { if(byte == "107"){
        cout << "\x6b";
    } else { if(byte == "108"){
        cout << "\x6c";
    } else { if(byte == "109"){
        cout << "\x6d";
    } else { if(byte == "110"){
        cout << "\x6e";
    } else { if(byte == "111"){
        cout << "\x6f";
    } else { if(byte == "112"){
        cout << "\x70";
    } else { if(byte == "113"){
        cout << "\x71";
    } else { if(byte == "114"){
        cout << "\x72";
    } else { if(byte == "115"){
        cout << "\x73";
    } else { if(byte == "116"){
        cout << "\x74";
    } else { if(byte == "117"){
        cout << "\x75";
    } else { if(byte == "118"){
        cout << "\x76";
    } else { if(byte == "119"){
        cout << "\x77";
    } else { if(byte == "120"){
        cout << "\x78";
    } else { if(byte == "121"){
        cout << "\x79";
    } else { if(byte == "122"){
        cout << "\x7a";
    } else { if(byte == "123"){
        cout << "\x7b";
    } else { if(byte == "124"){
        cout << "\x7c";
    } else { if(byte == "125"){
        cout << "\x7d";
    } else { if(byte == "126"){
        cout << "\x7e";
    } else { if(byte == "127"){
        cout << "\x7f";
    } else { if(byte == "128"){
        cout << "\x80";
    } else { if(byte == "129"){
        cout << "\x81";
    } else { if(byte == "130"){
        cout << "\x82";
    } else { if(byte == "131"){
        cout << "\x83";
    } else { if(byte == "132"){
        cout << "\x84";
    } else { if(byte == "133"){
        cout << "\x85";
    } else { if(byte == "134"){
        cout << "\x86";
    } else { if(byte == "135"){
        cout << "\x87";
    } else { if(byte == "136"){
        cout << "\x88";
    } else { if(byte == "137"){
        cout << "\x89";
    } else { if(byte == "138"){
        cout << "\x8a";
    } else { if(byte == "139"){
        cout << "\x8b";
    } else { if(byte == "140"){
        cout << "\x8c";
    } else { if(byte == "141"){
        cout << "\x8d";
    } else { if(byte == "142"){
        cout << "\x8e";
    } else { if(byte == "143"){
        cout << "\x8f";
    } else { if(byte == "144"){
        cout << "\x90";
    } else { if(byte == "145"){
        cout << "\x91";
    } else { if(byte == "146"){
        cout << "\x92";
    } else { if(byte == "147"){
        cout << "\x93";
    } else { if(byte == "148"){
        cout << "\x94";
    } else { if(byte == "149"){
        cout << "\x95";
    } else { if(byte == "150"){
        cout << "\x96";
    } else { if(byte == "151"){
        cout << "\x97";
    } else { if(byte == "152"){
        cout << "\x98";
    } else { if(byte == "153"){
        cout << "\x99";
    } else { if(byte == "154"){
        cout << "\x9a";
    } else { if(byte == "155"){
        cout << "\x9b";
    } else { if(byte == "156"){
        cout << "\x9c";
    } else { if(byte == "157"){
        cout << "\x9d";
    } else { if(byte == "158"){
        cout << "\x9e";
    } else { if(byte == "159"){
        cout << "\x9f";
    } else { if(byte == "160"){
        cout << "\xa0";
    } else { if(byte == "161"){
        cout << "\xa1";
    } else { if(byte == "162"){
        cout << "\xa2";
    } else { if(byte == "163"){
        cout << "\xa3";
    } else { if(byte == "164"){
        cout << "\xa4";
    } else { if(byte == "165"){
        cout << "\xa5";
    } else { if(byte == "166"){
        cout << "\xa6";
    } else { if(byte == "167"){
        cout << "\xa7";
    } else { if(byte == "168"){
        cout << "\xa8";
    } else { if(byte == "169"){
        cout << "\xa9";
    } else { if(byte == "170"){
        cout << "\xaa";
    } else { if(byte == "171"){
        cout << "\xab";
    } else { if(byte == "172"){
        cout << "\xac";
    } else { if(byte == "173"){
        cout << "\xad";
    } else { if(byte == "174"){
        cout << "\xae";
    } else { if(byte == "175"){
        cout << "\xaf";
    } else { if(byte == "176"){
        cout << "\xb0";
    } else { if(byte == "177"){
        cout << "\xb1";
    } else { if(byte == "178"){
        cout << "\xb2";
    } else { if(byte == "179"){
        cout << "\xb3";
    } else { if(byte == "180"){
        cout << "\xb4";
    } else { if(byte == "181"){
        cout << "\xb5";
    } else { if(byte == "182"){
        cout << "\xb6";
    } else { if(byte == "183"){
        cout << "\xb7";
    } else { if(byte == "184"){
        cout << "\xb8";
    } else { if(byte == "185"){
        cout << "\xb9";
    } else { if(byte == "186"){
        cout << "\xba";
    } else { if(byte == "187"){
        cout << "\xbb";
    } else { if(byte == "188"){
        cout << "\xbc";
    } else { if(byte == "189"){
        cout << "\xbd";
    } else { if(byte == "190"){
        cout << "\xbe";
    } else { if(byte == "191"){
        cout << "\xbf";
    } else { if(byte == "192"){
        cout << "\xc0";
    } else { if(byte == "193"){
        cout << "\xc1";
    } else { if(byte == "194"){
        cout << "\xc2";
    } else { if(byte == "195"){
        cout << "\xc3";
    } else { if(byte == "196"){
        cout << "\xc4";
    } else { if(byte == "197"){
        cout << "\xc5";
    } else { if(byte == "198"){
        cout << "\xc6";
    } else { if(byte == "199"){
        cout << "\xc7";
    } else { if(byte == "200"){
        cout << "\xc8";
    } else { if(byte == "201"){
        cout << "\xc9";
    } else { if(byte == "202"){
        cout << "\xca";
    } else { if(byte == "203"){
        cout << "\xcb";
    } else { if(byte == "204"){
        cout << "\xcc";
    } else { if(byte == "205"){
        cout << "\xcd";
    } else { if(byte == "206"){
        cout << "\xce";
    } else { if(byte == "207"){
        cout << "\xcf";
    } else { if(byte == "208"){
        cout << "\xd0";
    } else { if(byte == "209"){
        cout << "\xd1";
    } else { if(byte == "210"){
        cout << "\xd2";
    } else { if(byte == "211"){
        cout << "\xd3";
    } else { if(byte == "212"){
        cout << "\xd4";
    } else { if(byte == "213"){
        cout << "\xd5";
    } else { if(byte == "214"){
        cout << "\xd6";
    } else { if(byte == "215"){
        cout << "\xd7";
    } else { if(byte == "216"){
        cout << "\xd8";
    } else { if(byte == "217"){
        cout << "\xd9";
    } else { if(byte == "218"){
        cout << "\xda";
    } else { if(byte == "219"){
        cout << "\xdb";
    } else { if(byte == "220"){
        cout << "\xdc";
    } else { if(byte == "221"){
        cout << "\xdd";
    } else { if(byte == "222"){
        cout << "\xde";
    } else { if(byte == "223"){
        cout << "\xdf";
    } else { if(byte == "224"){
        cout << "\xe0";
    } else { if(byte == "225"){
        cout << "\xe1";
    } else { if(byte == "226"){
        cout << "\xe2";
    } else { if(byte == "227"){
        cout << "\xe3";
    } else { if(byte == "228"){
        cout << "\xe4";
    } else { if(byte == "229"){
        cout << "\xe5";
    } else { if(byte == "230"){
        cout << "\xe6";
    } else { if(byte == "231"){
        cout << "\xe7";
    } else { if(byte == "232"){
        cout << "\xe8";
    } else { if(byte == "233"){
        cout << "\xe9";
    } else { if(byte == "234"){
        cout << "\xea";
    } else { if(byte == "235"){
        cout << "\xeb";
    } else { if(byte == "236"){
        cout << "\xec";
    } else { if(byte == "237"){
        cout << "\xed";
    } else { if(byte == "238"){
        cout << "\xee";
    } else { if(byte == "239"){
        cout << "\xef";
    } else { if(byte == "240"){
        cout << "\xf0";
    } else { if(byte == "241"){
        cout << "\xf1";
    } else { if(byte == "242"){
        cout << "\xf2";
    } else { if(byte == "243"){
        cout << "\xf3";
    } else { if(byte == "244"){
        cout << "\xf4";
    } else { if(byte == "245"){
        cout << "\xf5";
    } else { if(byte == "246"){
        cout << "\xf6";
    } else { if(byte == "247"){
        cout << "\xf7";
    } else { if(byte == "248"){
        cout << "\xf8";
    } else { if(byte == "249"){
        cout << "\xf9";
    } else { if(byte == "250"){
        cout << "\xfa";
    } else { if(byte == "251"){
        cout << "\xfb";
    } else { if(byte == "252"){
        cout << "\xfc";
    } else { if(byte == "253"){
        cout << "\xfd";
    } else { if(byte == "254"){
        cout << "\xfe";
    } else { if(byte == "255"){
        cout << "\xff";
        // je potreba vsechny zavorky ukoncit :D
    } else { }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
    return 0;
}

// najde nasledujici ] na stejne urovni
// hleda v code od indexu starting
int findNext(string code, int starting){
    // aktualni instrukci neresime
    starting = starting + 1;

    // jak moc jsme zanoreni
    auto level = 0;
    int totalLen;
    totalLen = length(code);

    // hledame az dokonce
    for(int dummy; starting < totalLen; starting = starting + 1){

        // aktualne prohledavana instrukce
        string curr;
        curr = substr(code, starting, 1);
        if(curr == "["){
            // zvysime uroven
            level = level + 1;
        } else { if(curr == "]"){
            if(level == 0){
                // pokud jsme na stejne urovni, nasli jsme
                return starting;
            } else {
                // pokud ne, alespon dekrementujeme uroven
                level = level - 1;
            }
        } else{}}
    }

    // pokud jsme nenasli, je to chyba
    cout << "nemuzu najit koncovou zavorku, koncim!";
    return 0 / 0;
}

// najde predesle [ na stejne urovni
// hleda v code od indexu starting dozadu
int findPrevious(string code, int starting){
    // prohledavame od predesle instrukce
    starting = starting - 1;
    auto level = 0;

    //hledame az do zacatku
    for(int dummy; starting >= 0; starting = starting - 1){

        // aktualne prohledavana instrukce
        string curr;
        curr = substr(code, starting, 1);
        if(curr == "]"){
            // zvysime uroven
            level = level + 1;
        } else { if(curr == "["){
            if(level == 0){
                // pokud jsme na stejne urovni, nasli jsme
                return starting;
            } else {
                // jinak pouze snizime uroven
                level = level - 1;
            }
        } else {}}
    }
    cout << "nemuzu najit pocatecni zavorku, koncim!";
    return 0 / 0;
}

int interpret(string code){
	int len;
	len = length(code);

    // pamet
	string data = "000,";

    // aktualni velikost pameti
 	auto dSize = 1;

    // pointer na aktualni prvek
	auto dPointer = 0;

	for(int cPointer = 0; cPointer < len; cPointer = cPointer + 1){

        // aktualni instrukce
		string instr;
		instr = substr(code, cPointer, 1);
		if(instr == ">"){
            // posun pointeru doprava
			dPointer = dPointer + 1;

            // pokud jsme na neinicializovane pameti, inicializuj ji
			if(dPointer >= dSize){
				data = concat(data, "000,");
				dSize = dSize + 1;
			} else {}
		} else { if(instr == "<"){
            // posun pointeru doleva
			dPointer = dPointer - 1;
		} else { if(instr == "+"){
            // inkrementace aktualni bunky
            data = write(data, dPointer, 1);
        } else { if(instr == "-"){
            // dekrementace aktualni bunky
			data = write(data, dPointer, 0);
		} else { if(instr == "."){
            // vytiskni aktualni bunku
			auto p4 = dPointer * 4;
			string c;
			c = substr(data, p4, 3);
            int a;
			a = print(c);
		} else { if(instr == "["){
            // najdi hodnotu aktualni bunky
            auto p4 = dPointer * 4;
            string c;
            c = substr(data, p4, 3);

            // pokud je nulova, skaceme, jinak nic nedelame
            if(c == "000"){
                cPointer = findNext(code, cPointer);
            } else{}
        } else { if(instr == "]"){
            // najdi hodnotu aktualni bunku
            auto p4 = dPointer * 4;
            string c;
            c = substr(data, p4, 3);

            // pokud je hodnota nenulova, skaceme, jinak nic
            if(c != "000"){
                cPointer = findPrevious(code, cPointer);
            } else{}
        } else {
            // efektivni parsovani komentaru v brainfucku je umisteno zde
        }}}}}}}
	}
	return 0;
}

int main(){
    // s velkym podekovanim vypujceno z http://www.hevanet.com/cristofd/brainfuck/squares.b
	auto code = "++++[>+++++<-]>[<+++++>-]+<+[ >[>+>+<<-]++>>[<<+>>-]>>>[-]++>[-]+ >>>+[[-]++++++>>>]<<<[[<++++++++<++>>-]+<.<[>----<-]<] <<[>>>>>[>>>[-]+++++++++<[>-<-]+++++++++>[-[<->-]+[<<<]]<[>+<-]>]<<-]<<- ]";
	int a;
	a = interpret(code);
	return 0;
}
