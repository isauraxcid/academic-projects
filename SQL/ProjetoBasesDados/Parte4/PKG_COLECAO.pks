-----------------------------------------------------------------------------------------------------------------------
-- SIBD - 2025/2026 - Etapa 4 - Grupo 14
-----------------------------------------------------------------------------------------------------------------------
-- Isaura Cid    - 63761 - TP11
-- José Marques  - 63777 - TP11
-- Miguel Duarte - 60498 - TP15
-- Telmo Esteves - 64728 - TP15
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE PACKAGE pkg_colecao IS
-----------------------------------------------------------------------------------------------------------------------
-- Exceções
-----------------------------------------------------------------------------------------------------------------------
-- -20001 - Ano do álbum anterior ao início de atividade do artista
-- -20002 - Artista inexistente
-- -20003 - Utilizador deve ter 13 ou mais anos
-- -20004 - Artista com ISNI não encontrado
-- -20005 - Username já existe
-- -20006 - Erro inesperado ao obter dados para validação
-- -20007 - Utilizador não encontrado
-- -20008 - Álbum ja existe
-- -20009 - Data de posse anterior ao ano de lançamento
-- -20010 - Data de posse anterior ao ano em que utilizador fez 13 anos
-- -20011 - Registo de posse já existe
-- -20012 - Registo de posse não existe
-- -20013 - Utilizador não existe
-- -20014 - Álbum não existe
-- -20015 - Artista não existe
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_artista(
    isni_in   IN artista.isni%TYPE,
    nome_in   IN artista.nome%TYPE,
    inicio_in IN artista.inicio%TYPE);
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_album(
    ean_in     IN album.ean%TYPE,
    titulo_in  IN album.titulo%TYPE,
    tipo_in    IN album.tipo%TYPE,
    ano_in     IN album.ano%TYPE,
    artista_in IN album.artista%TYPE,
    suporte_in IN album.suporte%TYPE,
    versao_in  IN album.versao%TYPE := NULL);
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_utilizador(
    username_in   IN utilizador.username%TYPE,
    email_in      IN utilizador.email%TYPE,
    senha_in      IN utilizador.senha%TYPE,
    nascimento_in IN utilizador.nascimento%TYPE,
    artista_in    IN utilizador.artista%TYPE := NULL);
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION regista_posse(
    utilizador_in IN possui.utilizador%TYPE,
    album_in      IN possui.album%TYPE,
    desde_in      IN possui.desde%TYPE := SYSDATE)
    RETURN NUMBER;
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION remove_posse(
    utilizador_in IN possui.utilizador%TYPE,
    album_in      IN possui.album%TYPE)
    RETURN NUMBER;
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_utilizador(
    username_in IN utilizador.username%TYPE);
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_album(
    ean_in IN album.ean%TYPE);
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_artista(
    isni_in IN artista.isni%TYPE);
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION lista_albuns(
    utilizador_in IN utilizador.username%TYPE
  ) RETURN SYS_REFCURSOR;  
-----------------------------------------------------------------------------------------------------------------------
END pkg_colecao;
/