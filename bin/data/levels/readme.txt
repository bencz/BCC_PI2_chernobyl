como as fases funcionam atualmente:

**largura do mapa**: 32
**altura do mapa**: 18
**tamanho das tiles**: 32x32

apenas dois tilesets devem existir.
- **tilesetCollision** *(armazenado em data/bitmap/tilesetCollision.png)*, com dois blocos: o da esquerda para colisão e o da direita para bases. este tileset será exclusivamente usado na camada collision;
- **tileset** *(armazenado em data/bitmap/tileset.png)*, para tiles da fase em geral. este tileset será usado em qualquer outra camada menos a collision.

as camadas do arquivo são as seguintes, na ordem:
- **wires** *(do tipo camada de objeto)*: ela indica os circuitos (desenhados com polilinhas) do mapa, que serão alinhados com as bordas das tiles. o jogo espera que o fim do cabo de uma fase, quando no canto da tela, encaixe com o da fase que vem ao lado/acima/abaixo;
- **collision** *(do tipo camada de tile)*: nela, será indicado se há colisão naquela tile ou não. também haverá um tipo de tile indicando se há uma base ali, e bases são indicadas com esse tipo de tile desenhado em 2x2;
- **front** *(do tipo camada de tile)*: ela contém sprites que não são usados para colisão, mas para serem desenhados na tela. estes serão desenhados na frente do jogador;
- **back** *(do tipo camada de tile)*: assim como o front, mas estes serão desenhados atrás do jogador;
- **parallax** *(do tipo camada de tile)*: também serão desenhados atrás do jogador, mas, durante a transição entre dois mapas, ela anda mais lentamente que as outras camadas, dando a ilusão de q as tiles aqui estão mais longes da câmera (efeito de parallax, oras).

as variáveis que indicam como os mapas serão arranjados estão em **map.h**, sendo:
- **mapList**: array que indica os arquivos de mapa a serem utilizados;
- **mapGrid**: "matriz" com a ordem dos mapas (no caso cada número sendo o índice do mapa em mapList e mapGridWidth a largura dessa "matriz");
- **mapStartX** e **mapStartY**: posição que indica o mapa inicial do jogador, de acordo com a mapGrid (isso pode mudar, caso em algum momento haja sistema de save);
- **mapStartBase**: índice da base na qual o jogador iniciará. o índice pode ser descoberto varrendo o mapa (indicado em mapStartX/Y) de cima pra baixo, da esquerda pra direita, começando em 0.

tem uma variável extra em **level.c** que diz a resolução do *tilemap.png* (em tiles, não em pixels), então caso for alterar o tamanho dessa imagem não esquece de atualizar o código!!

eu recomendaria usar um dos .tmx atuais como base, já que eles estão na estrutura que o jogo espera. daí só alterar o *tileset.png* pra deixar do jeito certo. espero não dar muito trabalho, considerando as fases que já foram feitas antes desse arquivo...