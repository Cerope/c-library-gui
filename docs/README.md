# projet-ig-clean

INITIALISER LE DEPOT
	1) Créer un dépôt : 				git init
	1') Cloner le dépot distant : 			git clone username@host:/path


AJOUTER UNE FONCTION
	1) Créer une branche : 				git checkout -b <branch>
	2) Coder
	3) Ajouter des changements : 			git add .
	4) Valider les changements : 			git commit -m "Message"
	5) Envoyer les changements au dépôt : 		git push origin <branch>
	5) Après avoir fini, retourner sur master : 	git checkout master
	6) Fusionner master avec la branche : 		git merge <branch>
		6') En cas de conflit : 		git diff master <branch>
		6") Après les avoir réglés : 		git add <filename>
	7) Optionnel, supprimer la branche : 		git branch -d <branch>
