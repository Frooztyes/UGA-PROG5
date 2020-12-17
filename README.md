# UGA-PROG5 Projet 

Pour avoir une bonne gestion du projet et du dev de chacun, on va travailler avec des branches de developpement.

<img src="https://wac-cdn.atlassian.com/dam/jcr:746be214-eb99-462c-9319-04a4d2eeebfa/01.svg?cdnVersion=1377" width="70%" height="auto">

En bref, une branche permet de développer ses propres fonctionnalités, sans influer sur le travail des autres. Ça permet de gèrer efficacement les conflits et autres problèmes (éviter de supprimer le travail de chacun).

## Cloner le projet : 
<br>

>```git clone https://github.com/Frooztyes/UGA-PROG5.git```

<br>

## Faire un commit :
<br>

Voir si votre branche est derrière celle en ligne :
>```git status```

Si votre branche est derrière celle en ligne :
>```git pull```

Votre branche est à jour :
>```git add -A```\
>```git commit -m "message du commit"```\
>```git push```

<br>

## Créer une branche :
<br>

Crée une branche et s'y placer :
>```git checkout -b [nom de la branche]```\
>```git push origin [nom de la branche]```

Vérifier que l'on est sur sa branche :
>```git branch -a```\
>Affiche les branches et met une * devant la branche sur laquelle vous êtes.

Se mettre à jour par rapport à la branche main :
>```git rebase main [nom de la branche]```

Merge votre branche dans la main :
>Aller sur le site <a href="https://github.com/Frooztyes/UGA-PROG5.git"> (lien)</a>\
>Aller dans l'onglet "Pull requests"\
>Cliquer sur "New pull request" en vert\
>Choisir [nom de la branche] dans le bouton "compare"\
>Cliquer sur "Create pull request" en vert\
>Entrer une description et un commentaire (si besoin)
