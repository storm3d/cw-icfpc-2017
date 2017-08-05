Билд в Virtual Box
==================

1. Пробросить Shared Folder в виртуалку или скачать проект с GIT
2. ```cmake ./```
3. ```make```
4. Артефакты в папке ```bin```

Тестироние при помощи Lamduct
=============================

1. Устанавливаем Lamduct в виртуалке:
    ```opam pin add lambda-duct https://github.com/icfpcontest2017/lambda-duct.git```
2. ```cd``` в папку с бинарником punter
3. ```lamduct punter --log-level 3 --game-port 9998```