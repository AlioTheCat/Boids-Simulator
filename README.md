# Boids-Simulator

Un simulateur qui reproduit un comportement similaire à celui des nuées d'oiseaux.

## Principe

Les oiseaux de position et vitesse $(p_i, v_i)$ se déplacent selon 3 règles :
* **La règle de cohésion :** l'oiseau subit une accélération proportionnelle à $p_i - \frac{1}{N}\sum_{j=1}^N p_i$ (i.e il est attiré par la position moyenne des oiseaux.
* **La règle de séparation :** tous les oiseaux se répulsent par une accélération semblable à une interaction coulombienne : $a_{i \leftarrow j} \propto \frac{p_i-p_j}{||p_i-p_j||^3}$
* **La règle d'alignement:** une accélération corrective qui tend à aligner toutes les vitesses : $a_l\propto \frac{1}{N} \sum_{i=1}^N v_i$
* **Le vent :** Une règle supplémentaire qui s'ajoute en supplément de la règle d'alignement de sorte à uniformiser les vitesses, à la manière d'une force de frottement fluide : $a_{vent} \propto v_i-v_{vent}$

## Commandes

* Cliquer sur les boids pour les activer/désactiver. Ils changent d'apparence lorsque vous effectuez cette action.
* Utiliser les flèches directionnelles pour déplacer la fenêtre (si jamais les oiseaux sortent de l'écran)
* Appuyez sur "Espace" pour changer la direction du vent dans une direction aléatoire.
