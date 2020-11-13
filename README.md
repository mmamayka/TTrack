## Если совсем кратко

Репозиторий содержит набор готовых задач курса ТехноТрек:

0. **ttrack-lib** - ~~удобная~~ библиотека, собранная из наиболее часто импользоемого или потенциально используемого кода по мере решения следующих заданий

1. **SquareEquation** - решает квадратное уравнение.

2. **Onegin** - сортирует построчно тексты сначаоа и с конца.

3. **Stack** - шаблонный защищенный канарейками и хэшированием стэк, который так же осуществляет проверки на повторную инициализацию,
  хранит контекс создания экземпляра и вообще всячески защищает ~~меня~~ пользователя от выстрелов по ногам при его
  использовании в программе.
  
4. **Processor** - содержит эмулятор простого стэкового процессора, а так же ассемблер, дизасемблер для него

## ttrack-lib

Содержит набор средств сравнения переменных с плавающей точкой и некоторые необходимые целочисленные математические функции (**comp.h**);
средства для вывода отладочных сообщений и дампов состояний труктур данных, трассировки стека (**dbg.h**);
набор функций для решения линейных и квадратных уравнений (**eqsolve.h**);
набор функций для хэширования данных (**hash.h**);
собственный логгер (**log.h**);
шаблонный защищенный канарейками и хэшированием стэк, который так же осуществляет проверки на повторную инициализацию,
хранит контекс создания экземпляра и вообще всячески защищает ~~меня~~ пользователя от выстрелов по ногам при его
использовании в программе (**stack.h**);
набор функций и макросов для юнит-тестирования (**test.h**).

Сценарий для статической сборки библиотеки находится в сооствестсвующей директории **./ttrack-lib/makefile**, заголововочные файлы для
внешнего использования находятся в директории **./ttrack-lib/hdr**, бинарный файл находится в директории **./ttrack-lib/lib**.

## SquareEquation

Решает квадратное уравнение, принимая набор коэффициентов со стандартного ввода.

Сценарий сборки находится в сооствестсвующей директории **./SquareEquation/makefile**. Зависима от **ttrack-lib**. 
Перед сборкой программы необходимо собрать библиотеку **ttrack-lib**.

# Onegin

Сортирует построчно файл с именем **Shakespeare.txt** в прямом порядке, генерируя файл **Sorted.txt** и в обратном порядке - **FTOBSorted.txt**.

Сценарий сборки находится в сооствестсвующей директории **./Onegin/makefile**. Зависима от **ttrack-lib**. 
Перед сборкой программы необходимо собрать библиотеку **ttrack-lib**.

# Stack

Несколько тестов для вышеописанного стэка.

Сценарий сборки находится в сооствестсвующей директории **./Stack/makefile**. Зависима от **ttrack-lib**. 
Перед сборкой программы необходимо собрать библиотеку **ttrack-lib**.

# Processor

Ассембрирует, дизассемблирует и эмулирует выполнение кода для простого стекового процессора.

Сценарий сборки находится в сооствестсвующей директории **./Processor/*/makefile**. Зависима от **ttrack-lib** и **./Processor/LibCommon**. 
Перед сборкой программы необходимо собрать библиотеку **ttrack-lib** и билиотеку **./Processor/LibCommon**.
