--------------------------------------------------------------------------------------------------------------------------------------
-- SIBD - 2025/2026 - Etapa 3 - Grupo 14
--------------------------------------------------------------------------------------------------------------------------------------
-- Isaura Cid    - 63761 - TP11 - interrogação nº 3 (contribuiu 25% face ao total)
-- José Marques  - 63777 - TP11 - interrogação nº 1 (contribuiu 25% face ao total)
-- Miguel Duarte - 60498 - TP15 - interrogação nº 2 (contribuiu 25% face ao total)
-- Telmo Esteves - 64728 - TP15 - interrogação nº 4 (contribuiu 25% face ao total)
--------------------------------------------------------------------------------------------------------------------------------------
-- Artista (isni, nome, inicio)
-- Album (ean, titulo, tipo, ano, artista, suporte, versao)
-- Utilizador (username, email, senha, nascimento, artista)
-- Possui (utilizador, album, desde)
--------------------------------------------------------------------------------------------------------------------------------------
-- Interrogação 1
--------------------------------------------------------------------------------------------------------------------------------------
SELECT UT.username, UT.email, AL.titulo, AL.ean AS EAN13, AL.ano AS ano_lancamento, FLOOR(SYSDATE - PO.desde) AS num_dias_posse 
  FROM utilizador UT
       JOIN artista AR ON UT.artista = AR.isni
       JOIN possui PO ON PO.utilizador = UT.username
       JOIN album AL ON PO.album = AL.ean AND AL.artista = AR.isni
 WHERE (UT.nascimento = 1990) AND 
       (AR.nome = 'Day6') AND
       EXTRACT(YEAR FROM PO.desde) = 2025
ORDER BY UT.username ASC, 
         num_dias_posse DESC, 
         AL.ean DESC;
--------------------------------------------------------------------------------------------------------------------------------------
-- Interrogação 2
--------------------------------------------------------------------------------------------------------------------------------------
SELECT UT.username
  FROM utilizador UT
 WHERE (UT.email LIKE '%@gmail.com') 
       AND (UT.username NOT IN (SELECT PO.utilizador
                             FROM possui PO, album AL, artista AR
                            WHERE (PO.album = AL.ean) AND 
                                  (AL.artista = AR.isni) AND 
                                  (AR.nome = 'Dire Straits') AND 
                                  (TO_CHAR(PO.desde,'YYYY') BETWEEN '2000' AND '2020')
                                  GROUP BY PO.utilizador
                                  HAVING COUNT(*) > 3)) AND 
                                  (UT.username NOT IN (SELECT PO2.utilizador
                                                        FROM possui PO2, album AL2
                                                        WHERE (PO2.album = AL2.ean) AND 
                                                              (AL2.tipo = 'single')))
 ORDER BY UT.username ASC;
--------------------------------------------------------------------------------------------------------------------------------------
-- Interrogação 3
--------------------------------------------------------------------------------------------------------------------------------------
SELECT Ar.nome AS nome_artista, Ar.inicio AS ano_inicio_carreira
  FROM artista Ar
 WHERE EXISTS (SELECT *
                 FROM album AL, possui PO
                WHERE Al.artista = Ar.isni 
                      AND Al.ano >= EXTRACT(YEAR FROM SYSDATE) - 2)
       AND NOT EXISTS (SELECT *
                         FROM utilizador Ut
                        WHERE Ut.nascimento >= 2000
       AND NOT EXISTS (SELECT *
                         FROM album Al JOIN possui Po ON Po.album = Al.ean
                        WHERE Al.artista = Ar.isni 
                              AND Al.ano >= EXTRACT(YEAR FROM SYSDATE) - 2
                              AND Ut.username = Po.utilizador
                              AND TO_CHAR(Po.desde, 'HH24') BETWEEN '12' AND '19'))
ORDER BY Ar.nome ASC, 
         Ar.inicio DESC;
--------------------------------------------------------------------------------------------------------------------------------------
-- Interrogação 4
--------------------------------------------------------------------------------------------------------------------------------------
SELECT UT.username, decada_de_ano(UT.nascimento) AS decada, TO_CHAR(PO.desde, 'YYYY') AS ano, COUNT(PO.album) AS total_de_registos
  FROM utilizador UT, possui PO, album AL       
 WHERE PO.utilizador = UT.username
       AND PO.album = AL.ean
       AND AL.suporte = 'vinil'
       AND decada_de_ano(UT.nascimento) IN (1980, 1990)
GROUP BY
    UT.username,
    decada_de_ano(UT.nascimento),
    TO_CHAR(PO.desde, 'YYYY')
HAVING
    COUNT(PO.album) = (
        SELECT MAX(COUNT(PO2.album))
          FROM
            utilizador UT2,
            possui PO2,
            album AL2
         WHERE
            PO2.utilizador = UT2.username
            AND PO2.album = AL2.ean
            AND AL2.suporte = 'vinil'
            AND TO_CHAR(PO2.desde, 'YYYY') = TO_CHAR(PO.desde, 'YYYY')
            AND decada_de_ano(UT2.nascimento) = decada_de_ano(UT.nascimento)
    GROUP BY
        PO2.utilizador,
        decada_de_ano(UT2.nascimento),
        TO_CHAR(PO2.desde, 'YYYY')
    )
ORDER BY ano DESC,
         decada ASC;
--------------------------------------------------------------------------------------------------------------------------------------