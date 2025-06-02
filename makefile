# Compilation du compilateur et utilisation des fichiers tests pour générer le code assembleur
# 'make'
all:	compilateur tests

# Nettoyage des fichiers générés 
# 'make clean'
clean:
		@rm *.o 
		@rm ./s/*
		@rm tokeniser.cpp
		@rm compilateur

tokeniser.cpp:	tokeniser.l
		@flex++ -d -o tokeniser.cpp tokeniser.l

tokeniser.o:	tokeniser.cpp
		@g++ -c tokeniser.cpp

compilateur:	compilateur.cpp tokeniser.o
		@g++ -ggdb -o compilateur compilateur.cpp tokeniser.o

# Utilisation du compilateur sur tous les fichiers tests
tests:	compilateur 
		@mkdir -p ./s
		@echo ""
		@echo "🔧 Lancement des tests sur les fichiers .p"
		@echo ""
		@for file in ./p/*.p ; do \
			name=$$(basename $$file .p) ; \
			echo "🧪 Traitement de $$file → ./s/$$name.s" ; \
			./compilateur < $$file > ./s/$$name.s ; \
			gcc -ggdb -no-pie -fno-pie ./s/$$name.s -o ./s/$$name ; \
			echo "" ; \
		done
