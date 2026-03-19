# Тестовое задание для Effective Mobile  
Данный репозиторий был создан исключительно для выполнения ТЗ от Effective Mobile.  
Использовалась своя система сборки, но программу можно легко собрать используя нативный компилятор C++ с флагом -std=c++23
# Запуск  
Linux:
```sh
git clone https://github.com/maxsimilian560/interview-task.git
cd interview-task
./interview-task
```
Windows:
```cmd
git clone https://github.com/maxsimilian560/interview-task.git
cd interview-task
./interview-task.exe
```
По умолчанию программа читает inlet.in, но его можно изменить первым аргументом (./interview-task read-file.txt)  
Также программа может читать свои бинарные файлы (./interview-task read-file.bin)  

По умолчанию программа сохраняет результат в файл outlet.out, но это также можно изменить строго вторым аргументом (./interview-task read-file.txt save-file.bin)  

Подробнее см. вывод команды `./interview-task -h` или `./interview-task --help` для Linux, `./interview-task.exe -h` или `./interview-task.exe --help` для Windows  