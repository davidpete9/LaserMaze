#Felhasználói dolumentáció

##A LaserMaze játékról
A LaserMaze egy menüvezérelt logikai játék, melynek célja a lézerfény 
tükrökkel való irányítása, célba juttatása. A játékos pályánként
 különböző, egyre nehezedő kihívásokkal szembesül. 
 Minden pálya egy 5x5-ös nagyságú négyzetrács,
 melyben egy vagy több cél található. A játékos feladata, hogy a
 pályán lévő tükrök segítségével a célba juttassa a lézerfényt
 úgy, hogy: 
 * megválaszthatja azt, hogy honnan induljon ki a lézerfényt
 * amennyiben rendelkezésre áll a pályához megadott számú felhasználható tükör, akkor azokat lehelyezheti a pályára, tetszőleges szabad helyekre. 
 
 A játék 15 szintet tartalmaz, minden szint minimum 5 pályát. A játék célja ezeknek a pályáknak a végigvitele a legjobb eredménnyel.
 
 ##A játék használata
 Az alkalmazás megnyitását követően a főmenübe kerül a játékos, ahol kiválasztja, választhat
 a a játék, beállítások, és kilépés gombok között.
 Bármelyik gombra bal egérgombbal kattintva átirányít a megfelelő menüpontba.
 
 ##Beállítások
 * Itt a játékos visszaállíthatja a játékot alapállapotba, úgy hogy minden mentést töröl.
 * Be és ki lehet kapcsolni a fejlesztői módot:
    * Fejleszői módban bármely pály elérhető, játszható
    * A kihagyás gomb bármennyiszer használható
    * A könnyebb tesztelés miadt tettem be
    
 ##A játékmenü
 A játékmenüben ki van listázva mind a 15 pálya. A gombok háttérszíne azt mutatja, hogy az adott pálya játszható e, vagy sem
 * A gomb háttérszíne szürke, ha a szint még nincsen feloldva, ekkor a játékos az adott szinttel nem játszhat
 * A gomb háttérszíne zöld, ha már teljesítette a játékos a pályát. Bármikor újra lehet játszani ezeket a pályákat
 * A gomb háttérszíne kék, az az a pálya, amit a játékos még nem teljesített, de fel van oldva. (Mindig a legmagasabb számú elérhető szint)
 * A gomb háttérszíne piros, ha a szinthez tartozik mentés. Azt megnyitva folytatódik a játék ott, ahol legutóbb abba lett hagyva.
 Ha egy másik pályát indít el a játékos, akkor a mentés törlődik. Mindig a legutoljára játszott szintet menti el a rendszer.
 * Minden teljesített pálya felett jelzi csillagokkal a rendszer az eredményt. Hogy miért kapta a játékos a csillagot, az a pálya teljeítése után írja le a program 
  
 ##A játékmenet
 * Egy elérhető pályát megnyitva kirajzolódik a pálya. A nagy, 5x5-ös négyzetrácson találhatók a 
 tükrök, és a cél blokk, amelyeket a lézerfénynek kötelezően érintenie kell.
 
 
 * A jobb oldali oszlopban található egy a lézerlövő blokk, amit a játékos a megfelelő helyre mozgathat a pályán,
 a **bal egér gomb nyomva tartásával**. 
 Akkor helyezi le a négzetrácsra a blokkot 
 a program, ha a játékos egy üres négyzetrács fölött engedi el a blokkot.
 
 
 * A mozgatható blokkokat (melyeket a bal oldalról tett le a játékos) a **jobb egérgomb** nyomogatásával forgathat a játékos. Pl. a lézerkövő gombot mindig kell mozgatni és 
 forgatni.

 * A Tűz gomra kattintva a rendszer ellövi a lézert a lézerlövő blokktól, ha érintette az összes pályán lévő blokkot, és betalált
 a célba, akkor a pályát sikeresen teljesítette a játékos. Ekkor a rendszer a következő pályát automatikusan betölti 2 másodperces késleltetés után
 
*  A kihagyás gombot minden szinten csak egyszer lehet használni.

* A visszaállítás nevű gombra kattintva a pály a visszaáll alaphelyzetbe.

* A kilépés gomb visszaküld a játékmenübe, a játékállapotot ekkor elmenti

* Ha az összes pályát sikerült teljesíteni, akkor a játékos egy eredményjelző oldalra irányítja a rendszer, ahol láthatja  a 
játék eredményét (hány csillagot kapot, miért kapta őket.) Erről az oldalról tovább lehet navigálni a következő szintre,
vagy vissza lehet menni a játékmenübe.


 
 