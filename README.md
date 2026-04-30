# Назначение
Приложение предназначено для поиска текстовых файлов и их ранжирования в соответсвии с убыванием релевантности.
Приложение получает сразу несколько запросов, состоящих из ключевых слов и осуществляет поиск по ключивым словам в хранящихся локально текстовых файлах, перечень которых содержится в конфигурации.
Поисковый движок разработан в соответствиии с требованием ручного запуска на локальной машине. При необходимости его можно модифицировать с минимальными трудозатратами и использовать в составе более сложного продукта: desktop-приложения, монолитной ИС, микросервиса.

**Платформа:** PC

**ОС:** Windows 7-11, Linux, MacOS

**Язык:** Русский

# Стек используемых технологий
При разработке использованы:

**Языки:** C++

**Сторонние библиотеки:** nlohmann, GTests

**Система сборки:** CMake, Make

# Сборка и установка
Порядок сборки:
1) Клонировать репозиторий проекта:

```bash
git clone git@github.com:Arthur-Kotaro/search_engine.git
```

3) Перейти в корневой каталог проекта:

```bash
cd search_engine
```

4) Сгенерировать кэш:

Для конфигурации release:
```bash
cmake -S . -B build_release -DCMAKE_BUILD_TYPE=Release
```

Для конфигурации debug:
```bash
cmake -S . -B build_debug -DCMAKE_BUILD_TYPE=Debug
```

5) Скомпилировать и собрать проект:

В конфигурации release:
```bash
cmake --build build_release
```

В конфигурации debug:
```bash
cmake --build build_debug
```
6) Скопировать исполняемый файл search_engine в директорию, из которой будет запускаться программа:

```bash
cp /build_release/src/search_engine target_dir/
```

# Запуск и использование
Запуск приложения осуществляется командой:

```bash
./search_engine
```
Перед запуском предварительно требуется разместить в каталоге с исполняемым файлом файл конфигурации *config.json* и файл с поисковыми запросами *requests.json*.

В случае успеха будет создан файл *answers.json* с результатами поиска.


# Структура файла config.json
Конфигурационный файл содержит название поискового движка, его версию, интервалы времени, через которые обновляется поисковая база (чтение текстовых файлов и переиндексирование базы), максимальное количество файлов в ответе (если не указано, то значение выбирается равным пяти).

В поле "files" содержится список абсолютных или относительных путей к файлам с данными.

Пример содержимого файла config.json:

```json
{
    "config":
    {
        "name":        "SimpleTXTSearchEngine",
        "version":     "0.1",
        "max_responses": 5,
        "update_interval": 10
    },
    "files":
    [
        "../../res/output_Adventures_of_Huckleberry_Fin.txt",
        "../../res/output_Great_Expectations.txt",
        "../../res/output_Pride_and_Prejudice.txt",
        "../../res/output_Time_Machine.txt",
        "../../res/output_dracula.txt"
    ]
}
```

# Структура файла requests.json
Файл запросов содержит единственное поле "requests". Значением поля является список строк, каждая из которых представляет собой отдельный поисковый запрос.

Пример содержимого файла requests.json:

```json
{
        "requests" : [
                "apple pen penapple",
                "he saw her in her room",
                "sarcastic humor mr bennet william and lady lucas",
                "dracula british museum black moustaches london anywhere along carpathians",
                "have been work upon young man time traveller",
                "tremendous dip black hair charackter church",
                "that book made mr mark twain"
        ]
}
```

# Структура файла answers.json
 В файл answers.json записываются результаты работы поискового движка. Если файл уже существует, при очередном запросе файл будет перезаписан.

 Поля файла:
 - answers — базовое поле файла, содержит ответы на запросы.
 - requestNNN — идентификатор запроса, нумерация в соответсвии с очерёдностью следования запросов в поле requests файла requests.json.
- result – статус запроса. Принимает значение "true", если по данному запросу найден хотя бы один документ. В противном случае принимает значение "false", поле "relevance" отсутствует.
 - relevance — релевантность ответа. Содержит список пар "docID" и "rank".

 - <Идентификатор документа>("docid") — идентификатор документа, релевантного запросу. окументов исходя из порядка, в котором документы расположены в поле "files" в
 файле "config.json".

Пример описания файла answers.json:


```json
{
    "answers": {
        "request001": {
            "result": false
        },
        "request002": {
            "relevance": [
                {
                    "docID": 2,
                    "rank": 1.0
                },
                {
                    "docID": 0,
                    "rank": 0.5
                },
                {
                    "docID": 4,
                    "rank": 0.44999998807907104
                }
            ],
            "result": true
        },
        "request003": {
            "relevance": [
                {
                    "docID": 0,
                    "rank": 1.0
                },
                {
                    "docID": 2,
                    "rank": 0.8409090638160706
                },
                {
                    "docID": 1,
                    "rank": 0.8295454382896423
                },
                {
                    "docID": 4,
                    "rank": 0.5681818127632141
                },
                {
                    "docID": 3,
                    "rank": 0.46590909361839294
                }
            ],
            "result": true
        },
        "request004": {
            "relevance": [
                {
                    "docID": 4,
                    "rank": 1.0
                },
                {
                    "docID": 3,
                    "rank": 0.3529411852359772
                },
                {
                    "docID": 0,
                    "rank": 0.05882352963089943
                }
            ],
            "result": true
        },
        "request005": {
            "relevance": [
                {
                    "docID": 3,
                    "rank": 1.0
                },
                {
                    "docID": 1,
                    "rank": 0.5394737124443054
                },
                {
                    "docID": 2,
                    "rank": 0.3552631437778473
                },
                {
                    "docID": 4,
                    "rank": 0.25
                },
                {
                    "docID": 0,
                    "rank": 0.14473684132099152
                }
            ],
            "result": true
        },
        "request006": {
            "relevance": [
                {
                    "docID": 1,
                    "rank": 1.0
                }
            ],
            "result": true
        },
        "request007": {
            "relevance": [
                {
                    "docID": 2,
                    "rank": 1.0
                },
                {
                    "docID": 0,
                    "rank": 0.8600000143051147
                },
                {
                    "docID": 1,
                    "rank": 0.6600000262260437
                },
                {
                    "docID": 4,
                    "rank": 0.5400000214576721
                },
                {
                    "docID": 3,
                    "rank": 0.5
                }
            ],
            "result": true
        }
    }
}

```
