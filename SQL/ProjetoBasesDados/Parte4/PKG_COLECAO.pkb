-----------------------------------------------------------------------------------------------------------------------
-- SIBD - 2025/2026 - Etapa 4 - Grupo 14
-----------------------------------------------------------------------------------------------------------------------
-- Isaura Cid    - 63761 - TP11
-- José Marques  - 63777 - TP11
-- Miguel Duarte - 60498 - TP15
-- Telmo Esteves - 64728 - TP15
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE PACKAGE BODY pkg_colecao IS
-----------------------------------------------------------------------------------------------------------------------
-- Exceções
-----------------------------------------------------------------------------------------------------------------------
  e_utilizador_menor_13_anos       EXCEPTION;
  e_artista_nao_encontrado         EXCEPTION;
  e_data_posse_anterior            EXCEPTION;
  e_data_posse_anterior_13_anos    EXCEPTION;
  e_dados_inexistentes             EXCEPTION;
-----------------------------------------------------------------------------------------------------------------------
-- Procedimento auxiliar para mensagens inteligíveis
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE erro_constraint(
    p_codigo NUMBER,
    p_mensagem VARCHAR2
  ) IS
  BEGIN
    RAISE_APPLICATION_ERROR(p_codigo, p_mensagem);
  END erro_constraint;
-----------------------------------------------------------------------------------------------------------------------
-- Função auxiliar: conta álbuns de um utilizador
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION conta_albuns_utilizador(
    p_username utilizador.username%TYPE
  ) RETURN NUMBER IS
    v_total NUMBER;
  BEGIN
    SELECT COUNT(*)
      INTO v_total
      FROM possui
     WHERE utilizador = p_username;

    RETURN v_total;
  END conta_albuns_utilizador;
-----------------------------------------------------------------------------------------------------------------------
-- 1. Regista Artista
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_artista(
    isni_in   IN artista.isni%TYPE,
    nome_in   IN artista.nome%TYPE,
    inicio_in IN artista.inicio%TYPE
  ) IS
  BEGIN
    INSERT INTO artista (isni, nome, inicio)
    VALUES (isni_in, nome_in, inicio_in);

  EXCEPTION
    WHEN DUP_VAL_ON_INDEX THEN
      erro_constraint(-20015, 'Artista ja existe');
    WHEN OTHERS THEN
      erro_constraint(-20006, 'Erro inesperado ao registar artista');
  END regista_artista;
-----------------------------------------------------------------------------------------------------------------------
-- 2. Regista Álbum
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_album(
    ean_in     IN album.ean%TYPE,
    titulo_in  IN album.titulo%TYPE,
    tipo_in    IN album.tipo%TYPE,
    ano_in     IN album.ano%TYPE,
    artista_in IN album.artista%TYPE,
    suporte_in IN album.suporte%TYPE,
    versao_in  IN album.versao%TYPE
  ) IS
    v_inicio artista.inicio%TYPE;
  BEGIN
    SELECT inicio
      INTO v_inicio
      FROM artista
     WHERE isni = artista_in;

    IF ano_in < v_inicio THEN
      erro_constraint(-20001, 'Ano do album anterior ao inicio de atividade do artista');
    END IF;

    INSERT INTO album (ean, titulo, tipo, ano, artista, suporte, versao)
    VALUES (ean_in, titulo_in, tipo_in, ano_in, artista_in, suporte_in, versao_in);

  EXCEPTION
    WHEN NO_DATA_FOUND THEN
      erro_constraint(-20002, 'Artista inexistente');
    WHEN DUP_VAL_ON_INDEX THEN
      erro_constraint(-20008, 'Album ja existe');
    WHEN OTHERS THEN
      erro_constraint(-20006, 'Erro inesperado ao registar album');
  END regista_album;
-----------------------------------------------------------------------------------------------------------------------
-- 3. Regista Utilizador
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE regista_utilizador(
    username_in   IN utilizador.username%TYPE,
    email_in      IN utilizador.email%TYPE,
    senha_in      IN utilizador.senha%TYPE,
    nascimento_in IN utilizador.nascimento%TYPE,
    artista_in    IN utilizador.artista%TYPE
  ) IS
    v_idade NUMBER;
    v_count NUMBER;
  BEGIN
    v_idade := EXTRACT(YEAR FROM SYSDATE) - nascimento_in;

    IF v_idade < 13 THEN
      RAISE e_utilizador_menor_13_anos;
    END IF;

    IF artista_in IS NOT NULL THEN
      SELECT COUNT(*)
        INTO v_count
        FROM artista
       WHERE isni = artista_in;

      IF v_count = 0 THEN
        RAISE e_artista_nao_encontrado;
      END IF;
    END IF;

    INSERT INTO utilizador (username, email, senha, nascimento, artista)
    VALUES (username_in, email_in, senha_in, nascimento_in, artista_in);

  EXCEPTION
    WHEN e_utilizador_menor_13_anos THEN
      erro_constraint(-20003, 'Utilizador deve ter 13 ou mais anos');
    WHEN e_artista_nao_encontrado THEN
      erro_constraint(-20004, 'Artista nao encontrado');
    WHEN DUP_VAL_ON_INDEX THEN
      erro_constraint(-20005, 'Username ja existe');
    WHEN OTHERS THEN
      erro_constraint(-20006, 'Erro inesperado ao registar utilizador');
  END regista_utilizador;
-----------------------------------------------------------------------------------------------------------------------
-- 4. Regista Posse
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION regista_posse(
    utilizador_in IN possui.utilizador%TYPE,
    album_in      IN possui.album%TYPE,
    desde_in      IN possui.desde%TYPE
  ) RETURN NUMBER IS
    v_ano_album NUMBER;
    v_nasc      NUMBER;
    v_ano_posse NUMBER;
  BEGIN
    SELECT ano INTO v_ano_album FROM album WHERE ean = album_in;
    SELECT nascimento INTO v_nasc FROM utilizador WHERE username = utilizador_in;

    v_ano_posse := EXTRACT(YEAR FROM desde_in);

    IF v_ano_posse < v_ano_album THEN
      erro_constraint(-20009, 'Data de posse anterior ao ano de lancamento');
    END IF;

    IF v_ano_posse < v_nasc + 13 THEN
      erro_constraint(-20010, 'Data de posse anterior ao ano em que utilizador fez 13 anos');
    END IF;

    INSERT INTO possui (utilizador, album, desde)
    VALUES (utilizador_in, album_in, desde_in);

    RETURN conta_albuns_utilizador(utilizador_in);

  EXCEPTION
    WHEN NO_DATA_FOUND THEN
      erro_constraint(-20006, 'Erro inesperado ao obter dados para validacao');
    WHEN DUP_VAL_ON_INDEX THEN
      erro_constraint(-20011, 'Registo de posse ja existe');
  END regista_posse;
-----------------------------------------------------------------------------------------------------------------------
-- 5. Remove Posse
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION remove_posse(
    utilizador_in IN possui.utilizador%TYPE,
    album_in      IN possui.album%TYPE
  ) RETURN NUMBER IS
  BEGIN
    DELETE FROM possui
     WHERE utilizador = utilizador_in
       AND album = album_in;

    IF SQL%ROWCOUNT = 0 THEN
      erro_constraint(-20012, 'Registo de posse nao existe');
    END IF;

    RETURN conta_albuns_utilizador(utilizador_in);
  END remove_posse;
-----------------------------------------------------------------------------------------------------------------------
-- 6. Remove Utilizador
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_utilizador(
    username_in IN utilizador.username%TYPE
  ) IS
    v_dummy NUMBER;
  BEGIN
    FOR r IN (SELECT album FROM possui WHERE utilizador = username_in) LOOP
      v_dummy := remove_posse(username_in, r.album);
    END LOOP;

    DELETE FROM utilizador WHERE username = username_in;

    IF SQL%ROWCOUNT = 0 THEN
      erro_constraint(-20013, 'Utilizador nao existe');
    END IF;
  END remove_utilizador;
-----------------------------------------------------------------------------------------------------------------------
-- 7. Remove Álbum
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_album(
    ean_in IN album.ean%TYPE
  ) IS
    v_dummy NUMBER;
  BEGIN
    FOR r IN (SELECT utilizador FROM possui WHERE album = ean_in) LOOP
      v_dummy := remove_posse(r.utilizador, ean_in);
    END LOOP;

    DELETE FROM album WHERE ean = ean_in;

    IF SQL%ROWCOUNT = 0 THEN
      erro_constraint(-20014, 'Album nao existe');
    END IF;
  END remove_album;
-----------------------------------------------------------------------------------------------------------------------
-- 8. Remove Artista
-----------------------------------------------------------------------------------------------------------------------
  PROCEDURE remove_artista(
    isni_in IN artista.isni%TYPE
  ) IS
  BEGIN
    FOR r IN (SELECT ean FROM album WHERE artista = isni_in) LOOP
      remove_album(r.ean);
    END LOOP;

    DELETE FROM artista WHERE isni = isni_in;

    IF SQL%ROWCOUNT = 0 THEN
      erro_constraint(-20015, 'Artista nao existe');
    END IF;
  END remove_artista;
-----------------------------------------------------------------------------------------------------------------------
-- 9. Lista Álbuns de um Utilizador
-----------------------------------------------------------------------------------------------------------------------
  FUNCTION lista_albuns(
    utilizador_in IN utilizador.username%TYPE
  ) RETURN SYS_REFCURSOR IS
    v_cursor SYS_REFCURSOR;
    v_count  NUMBER;
  BEGIN
    SELECT COUNT(*)
      INTO v_count
      FROM utilizador
     WHERE username = utilizador_in;

    IF v_count = 0 THEN
      erro_constraint(-20007, 'Utilizador nao encontrado');
    END IF;

    OPEN v_cursor FOR
      SELECT
        al.ean,
        al.titulo,
        al.tipo,
        al.ano,
        al.suporte,
        CASE WHEN al.versao IS NULL THEN '-' ELSE al.versao END AS versao,
        CASE WHEN u.artista = ar.isni THEN ar.nome || '*' ELSE ar.nome
        END AS nome_artista
      FROM possui p
      JOIN album al      ON al.ean = p.album
      JOIN artista ar    ON ar.isni = al.artista
      JOIN utilizador u  ON u.username = p.utilizador
     WHERE p.utilizador = utilizador_in
     ORDER BY
       al.ano DESC,
       ar.nome ASC,
       al.titulo ASC;

    RETURN v_cursor;
  END lista_albuns;
-----------------------------------------------------------------------------------------------------------------------
END pkg_colecao;
/