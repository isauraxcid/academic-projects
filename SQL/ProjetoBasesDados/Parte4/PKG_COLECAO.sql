-----------------------------------------------------------------------------------------------------------------------
-- SIBD - 2025/2026 - Etapa 4 - Grupo 14
-----------------------------------------------------------------------------------------------------------------------
-- Isaura Cid    - 63761 - TP11 - procedimentos regista_artista e regista_album (contribuiu 25% face ao total)
-- José Marques  - 63777 - TP11 - procedimento regista_utilizador e função regista_posse (contribuiu 25% face ao total)
-- Miguel Duarte - 60498 - TP15 - função remove_posse e procedimento remove_utilizador (contribuiu 25% face ao total)
-- Telmo Esteves - 64728 - TP15 - procedimentos remove_album e remove_artista (contribuiu 25% face ao total)
-- A função remove_artista foi feita em conjunto, bem como a Demonstração.
-----------------------------------------------------------------------------------------------------------------------
-- Destacar que foram feitas reuniões periódicas, com o fim de discussão e desenvolvimento do trabalho.
-----------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
SET SERVEROUTPUT ON
-----------------------------------------------------------------------------------------------------------------------
-- Inserir artistas 
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  DBMS_OUTPUT.PUT_LINE('Registar artistas');
  
  pkg_colecao.regista_artista('0000000000000001', 'Metallica', 1981);
  pkg_colecao.regista_artista('0000000000000002', 'Coldplay', 1996);

  COMMIT;
  DBMS_OUTPUT.PUT_LINE('Artistas registados com sucesso.');
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - ter um artista duplicado
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.regista_artista('0000000000000001', 'Metallica', 1981);
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Inserir álbuns 
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  DBMS_OUTPUT.PUT_LINE('Registar albuns');

  pkg_colecao.regista_album('1234567890123', 'Black Album', 'LP', 1991, '0000000000000001', 'vinil', NULL);
  pkg_colecao.regista_album('1234567890124', 'Parachutes', 'EP', 2000, '0000000000000002', 'CD', NULL);

  COMMIT;
  DBMS_OUTPUT.PUT_LINE('Albuns registados com sucesso.');
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - ano do álbum anterior ao início do artista
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.regista_album('1234567890125', 'Album Errado', 'LP', 1980, '0000000000000001', 'vinil', NULL);
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Inserir utilizadores
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  DBMS_OUTPUT.PUT_LINE('Registar utilizadores');

  pkg_colecao.regista_utilizador('joao', 'joao@mail.pt', '1234', 2000, '0000000000000001');
  pkg_colecao.regista_utilizador('ana', 'ana@mail.pt', 'abcd', 1995, NULL);

  COMMIT;
  DBMS_OUTPUT.PUT_LINE('Utilizadores registados com sucesso.');
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - utilizador com idade inferior a 13 anos
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.regista_utilizador('miudo', 'miudo@mail.pt', '123', 2015, NULL);
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
--  Registar posse de álbuns
-----------------------------------------------------------------------------------------------------------------------
DECLARE
  v_total NUMBER;
BEGIN
  DBMS_OUTPUT.PUT_LINE('Registar posse');

  v_total := pkg_colecao.regista_posse('joao', '1234567890123', DATE '2018-01-01');
  DBMS_OUTPUT.PUT_LINE('O Joao tem ' || v_total || ' albuns na coleção.');

  v_total := pkg_colecao.regista_posse('ana', '1234567890124', DATE '2020-01-01');
  DBMS_OUTPUT.PUT_LINE('A Ana tem ' || v_total || ' albuns na coleção.');

  COMMIT;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - registo de posse duplicado
-----------------------------------------------------------------------------------------------------------------------
DECLARE
  v_total NUMBER;
BEGIN
  v_total := pkg_colecao.regista_posse('joao', '1234567890123', DATE '2018-01-01');
  DBMS_OUTPUT.PUT_LINE('O Joao tem ' || v_total || ' albuns na coleção.');
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Listar álbuns de um utilizador
-----------------------------------------------------------------------------------------------------------------------
DECLARE
  c SYS_REFCURSOR;
  v_ean     CHAR(13);
  v_titulo  VARCHAR2(80);
  v_tipo    VARCHAR2(6);
  v_ano     NUMBER;
  v_suporte VARCHAR2(7);
  v_versao  VARCHAR2(80);
  v_artista VARCHAR2(80);
BEGIN
  DBMS_OUTPUT.PUT_LINE('Listar albuns do Joao:');
  
  c := pkg_colecao.lista_albuns('joao');

  LOOP
    FETCH c INTO v_ean, v_titulo, v_tipo, v_ano, v_suporte, v_versao, v_artista;
    EXIT WHEN c%NOTFOUND;
    DBMS_OUTPUT.PUT_LINE(v_ean || ' | ' || v_titulo || ' | ' || v_tipo || ' | ' || v_ano || ' | ' || v_suporte || ' | ' || NVL(v_versao,'-') || ' | ' || v_artista);
  END LOOP;

  CLOSE c;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Remover posse
-----------------------------------------------------------------------------------------------------------------------
DECLARE
  v_total NUMBER;
BEGIN
  DBMS_OUTPUT.PUT_LINE('Remover posse de album');
  
  v_total := pkg_colecao.remove_posse('joao', '1234567890123');
  DBMS_OUTPUT.PUT_LINE('Joao tem agora ' || v_total || ' albuns.');

  COMMIT;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - remover uma posse que nao existe
-----------------------------------------------------------------------------------------------------------------------
DECLARE
  v_total NUMBER;
BEGIN
  v_total := pkg_colecao.remove_posse('joao', '1234567890123');
  DBMS_OUTPUT.PUT_LINE('Joao tem agora ' || v_total || ' albuns na coleção.');

EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Remover utilizador
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_utilizador('ana');
  COMMIT;
END;
/
BEGIN
  pkg_colecao.remove_utilizador('joao');
  COMMIT;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - Utilizador inexistente
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_utilizador('ana');
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
--  Remover álbum
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_album('1234567890124');
  COMMIT;
END;
/
BEGIN
  pkg_colecao.remove_album('1234567890123');
  COMMIT;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - álbum inexistente
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_album('1234567890124');
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Remover artista
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_artista('0000000000000001');
  COMMIT;
END;
/
-----------------------------------------------------------------------------------------------------------------------
-- Testar erro - Artista inexistente
-----------------------------------------------------------------------------------------------------------------------
BEGIN
  pkg_colecao.remove_artista('0000000000000001');
EXCEPTION
  WHEN OTHERS THEN
    DBMS_OUTPUT.PUT_LINE('ERRO ESPERADO: ' || SQLERRM);
END;
/