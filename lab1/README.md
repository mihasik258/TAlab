Разработать распознаватель на базе конечного автомата, который проверяет корректность вводимых строк удовлетворяющих следующему шаблону. Cтроки вводятся с клавиатуры или из файла. Для каждой сроки вывести на экран либо сохранить в файл результат  проверки ее корректности:

 

Шаблон:

ftp://имя_пользователя@имя_сервера.имя_домена.имя_зоны

 

ftp:// -  обязательный заголовок строки;

имя_пользователя – обязательное поле, строка литералов [a-z] в любом регистре длиной до 20 символов (распознаватель не должен быть чувствителен к регистру);

имя_сервера – обязательное поле, строка литералов [a-z] в любом регистре длиной до 20 символов  (распознаватель не должен быть чувствителен к регистру);

имя_домена – необязательное поле, представляет собой строки литералов [a-z] в любом из регистров длиной до 20 символов, разделенные символом ‘.’;

имя_зоны – обязательное поле, представляет собой строки литералов [a-z] в любом из регистров длиной до 5 символов.

 

Собрать и вывести статистку использования для корректных строк имен пользователей в виде: имя_пользователя – количество использований.

Результат вывести после обработки всех строк на экран или в файл.
