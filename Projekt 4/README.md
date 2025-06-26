# Projekt 4 - ramię robota

>*Krzysztof&nbsp;Chojka&nbsp;203183, Daniel&nbsp;Chrzanowski&nbsp;203381* **ACIR&nbsp;1B**

## Opis sterowania programem

**Tryb manualny:**
- Strzałki `▲ ▼ ◄ ►` – sterowanie ramionami robota
- Klawisz `P` – podniesienie klocka
- Klawisz `O` – odłożenie klocka w aktualnej pozycji końcówki ramienia
- Klawisz `R` – rozpoczęcie/zakończenie nagrywania ruchów ramienia
- Klawisz `F` – odtwarzanie nagranych ruchów (automatyczne wykonanie zapamiętanej sekwencji)

**Tryb automatyczny oraz obsługa GUI:**
- Przycisk `Wieża 6` – zbuduj automatycznie wieżę z 6 klocków
- Przycisk `Wieża 3` – automatyczne przeniesienie 3 klocków
- Przycisk `Waga +` / `Waga -` – zmiana domyślnej masy nowego klocka
- Przycisk `Rozmiar +` / `Rozmiar -` – zmiana domyślnego rozmiaru nowego klocka
- Przycisk `Dodaj Kwadrat` / `Dodaj Koło` / `Dodaj Prostokąt` / `Dodaj Trójkąt` – dodanie nowego klocka wybranego kształtu na planszę
- Przycisk `Wyczyść` – usuń wszystkie klocki

**Obsługa błędów i ograniczeń:**
- Program wyświetla komunikaty, gdy blok jest za ciężki lub za lekki, lub gdy próbujemy podnieść kolejny klocek, mając już jeden podniesiony.
- Nie można ustawić wieży o wysokości większej niż zasięg ramienia.

## Kompilowanie programu
```
cmake -S . -B build
cmake --build build
.\build\RobotArmSim.exe
```

## Wymagania ⬜ / ✅

✅ Z wykorzystaniem ramienia robota zbuduj wieżę złożoną z 6 ustawionych na sobie klocków (kształt klocka - kwadrat).  
✅Z wykorzystaniem ramienia robota zbuduj wieżę złożoną z 6 ustawionych na sobie klocków (kształt klocka - koło).  
✅ Z wykorzystaniem ramienia robota zbuduj wieżę złożoną z 6 ustawionych na sobie klocków (kształt klocka - trójkąt).  
✅ Z wykorzystaniem ramienia robota zbuduj wieżę złożoną z 6 ustawionych na sobie klocków (kształt klocka - prostokąt).  
✅ Zdefiniuj wagę wykorzystywanych w programie bloków. Zadaniem robota jest przeniesienie 3 bloków z jednego miejsca na drugie. Robot posiada ograniczenia co do masy przenoszonego bloku. Jeśli masa została przekroczona – użytkownik powinien otrzymać stosowną informację. W przypadku przekroczenia wartości maksymalnego udźwigu robot pomija blok i przenosi tylko te które jest w stanie.  
✅ Zdefiniuj wagę wykorzystywanych w programie bloków. Zadaniem robota jest przeniesienie 3 bloków z jednego miejsca na drugie. Robot posiada ograniczenia co do masy minimalnej i maksymalnej przenoszonego bloku. Jeśli masa przenoszonego elementu mieści się w stosownym przedziale robot przenosi element z jednego miejsca na drugie. W przypadku przekroczenia wartości in-plus, in-minus użytkownik powinien otrzymać stosowną informację.  
⬜ Robot ma dostępny zbiór elementów o różnej wysokości. Na podstawie położenia ramienia sprawdza wysokość elementów i ustawia je w kolejności od najwyższego do najniższego.  
⬜ Robot ma dostępny zbiór elementów o różnej wysokości. Na podstawie położenia ramienia sprawdza wysokość elementów i ustawia je w kolejności od najniższego do najwyższego.  
⬜ Robot ma dostępny zbiór elementów o różnej wysokości. Na podstawie położenia ramienia sprawdza wysokość elementów i ustawia je w następującej kolejności (piramidka, największy po środku).  
⬜ Robot ma dostępny zbiór 4 elementów o różnej masie. Na podstawie tej informacji ustawia elementy w kolejności od najcięższego do najlżejszego.  
⬜ Robot ma dostępny zbiór 4 elementów o różnej masie. Na podstawie tej informacji ustawia elementy w kolejności od najlżejszego do najcięższego.  

## TODO:
- ~~Dopisać komunikat dla zbyt lekkiego bloczka~~
- Zamiast wyświetlać numer bloczka, wyświetlać na nim jego masę
- ~~Uniemożliwić podniesienie bloczka jeśli już jakiś bloczek jest podniesiony~~
- Dodać zabezpieczenie przy generowaniu klocków, żeby nie ustawiło wyższej wierzy niż zasięg ramienia
- ~~Dodać obsługę błędu gdy R1Target wyrzuci pozycje (-1) (Używane w funkcji: moveToTarget())- Podano błędne ID bloku~~
- Dodać możliwość edycji bloczka? Wymagające obliczeniowo do zrobienia, może spowolnić program, zamiast tego można dać opcje usuwania i dodawania pojedynczych bloczków, ale gdzie wtedy miałoby je generować? 
- ~~Sprawdzanie osiągalności bloczka w funkcji R1Target() - czy potrzebne? jeśli nie będzie możliwości wygenerować bloczka do którego ramie nie sięgnie?~~
- ~~Naprawić sprawdzanie osiągalności bloczka - aktualnie nie działa, a jak się okazało jest kluczowe dla działania programu~~
- ~~Dodać animacje stawiania bloczka przy budowaniu wieży~~