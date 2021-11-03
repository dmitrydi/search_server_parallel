# Поисковая система
Класс `SearchServer` для поиска наиболее релевантных документов в базе по ключевым словам. Результаты поиска для поискового запроса записываются в выходной поток в виде сторки  
	
		"query text": {"docid": n1, "hitcount": m1}, {"docid": n2, "hitcount": m2}, ...
	
где `docid` - порядковый номер документа в базе, `hitcount` - суммарное количество вхождений слов из запроса `query text` в документ.  
Класс позволяет производить обновление базы документов и обрабатывать несколько потоков поисковых запросов в параллельных потоках.  
Маскимальное число хранимых документов составляет 50'000, максимальное количество наиболее релевантных документов в поисковой выдаче равно 5 (см. ниже).

# Установка
### Требования
Поддержка компиллятором стандарта C++17  

# Использование
Конструктор класса принимает поток ввода, содержащий базу документов. Документ - это строка входного потока. Внутри строки слова отделены друг от друга пробелами.  
Слова состоят из строчных латинских букв.  

### Инициализация
Пример инициализации экземпляра `SearchServer`:  

	string docs;
	docs += "london is the capital of great britain\n";
	docs += "i am travelling down the river\n";
	docs += "  here     we    go             ";
	istringstream document_input(docs);
	SearchServer srv(document_input);

Созданный таким образом экземпляр `srv` содердит 3 документа.  
Максимальное число хранимых в базе документов задается константой `SearchServer::MAX_DOCS`, максимальное количество документов в выдаче - `SearchServer::MAX_OUTPUT`.  

### Поиск
Поиск осуществляется с помощью вызова метода `void AddQueriesStream(std::istream& query_input, std::ostream& search_results_output)`: 

	const string doc1 = "london is the capital of great britain";
	const string doc2 = "moscow is the capital of the russian federation";
	istringstream doc_input(doc1 + '\n' + doc2);
	SearchServer srv(doc_input);

	const string query = "the capital";
	istringstream query_input(query);
	srv.AddQueriesStream(query_input, cout);

Вывод:  

	the capital: {docid: 1, hitcount: 3} {docid: 0, hitcount: 2}

Класс поддерживает параллельную обработку поисковых запросов:

	SearchServer srv(doc_input);
	vector<pair<istringstream, ostringstream*>> streams;
	...
	for (auto& [input, output] : streams)
		srv.AddQueriesStream(input, *output);

При этом общее время обработки запросов меньше суммы времен обработки каждого отдельного запроса.  

### Обновление базы документов

Обновление базы документов производится при помощи метода `void UpdateDocumentBase(istream& document_input)`. При этом старая база документов полностью затирается:  

	const string doc1 = "london is the capital of great britain";
	const string doc2 = "moscow is the capital of the russian federation";
	istringstream doc_input1(doc1 + '\n' + doc2);
	SearchServer srv(doc_input1);

	const string query = "the capital";
	istringstream query_input1(query);
	srv.AddQueriesStream(query_input1, cout);

	istringstream doc_input2(doc2 + '\n' + doc1);
	srv.UpdateDocumentBase(doc_input2);
	istringstream query_input2(query);
	srv.AddQueriesStream(query_input2, cout);

Вывод:

	the capital: {docid: 1, hitcount: 3} {docid: 0, hitcount: 2}
	the capital: {docid: 0, hitcount: 3} {docid: 1, hitcount: 2}

Обновление базы документов также может происходить параллельно с обработкой поисковых запросов. При этом возможно, что запросы, созданные до обновления базы, будут обработаны с использованием новой базы.  
