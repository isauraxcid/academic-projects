-------------------------------------------------------------------------------------------
-- SIBD, 2025/2026, G14.
-------------------------------------------------------------------------------------------
-- Isaura Cid    - 63761 - TP11 - versao, supfisico e tabela_em (25%)
-- José Marques  - 63777 - TP11 - artista, solista e grupo (25%)
-- Miguel Duarte - 60498 - TP15 - album, refere, interpretado (25%)
-- Telmo Esteves - 64728 - TP15 - possui, utilizador, favorito e lista_personalizada (25%)
-------------------------------------------------------------------------------------------
-- RIAs suportadas: 1, 2, 7, 10, 11, 12, 13, 14.
-- RIAs não suportadas: 8, 15, 16, 17.
-- Adicionámos o CHECK (ano_de_lancamento > 1899) para reforçar integridade.
-------------------------------------------------------------------------------------------
ALTER SESSION SET NLS_DATE_FORMAT = 'DD.MM.YYYY';
-------------------------------------------------------------------------------------------
DROP TABLE interpretado;
DROP TABLE grupo;
DROP TABLE solista;
DROP TABLE favorito;
DROP TABLE possui;
DROP TABLE artista CASCADE CONSTRAINTS;
DROP TABLE refere;
DROP TABLE lista_personalizada;
DROP TABLE utilizador CASCADE CONSTRAINTS;
DROP TABLE versao;
DROP TABLE tabela_em;
DROP TABLE album;
DROP TABLE supfisico;
-------------------------------------------------------------------------------------------
CREATE TABLE supfisico (
    tipo_sup        VARCHAR (10),
    --
    CONSTRAINT pk_supfisico
        PRIMARY KEY (tipo_sup),
    --
    CONSTRAINT ck_tipo_supfisico 
        CHECK (Lower(tipo_sup) IN ('cd', 'vinil', 'cassete'))  --RIA-11
);
-------------------------------------------------------------------------------------------
CREATE TABLE album (
    mbid                 VARCHAR(36),   
    titulo               VARCHAR(30) 	  CONSTRAINT nn_album_titulo 			        NOT NULL,
    tipo                 VARCHAR(10)  	CONSTRAINT nn_album_tipo			          NOT NULL,
    ano_de_lancamento    NUMBER(4)	  	CONSTRAINT nn_album_ano_de_lancamento		NOT NULL,
    -- 
    CONSTRAINT pk_album
        PRIMARY KEY (mbid),
    --
    CONSTRAINT ck_album_mbid                 
        CHECK (REGEXP_LIKE(mbid, '^[A-Za-z0-9]+$') AND LENGTH(mbid) = 36),   -- RIA-1
    --
    CONSTRAINT ck_album_tipo      
        CHECK (tipo IN ('single', 'ep', 'lp')),   -- RIA-2
    --
    CONSTRAINT ck_album_ano_de_lancamento
        CHECK (ano_de_lancamento >= 1900)
);
-------------------------------------------------------------------------------------------
CREATE TABLE tabela_em (
    supfisico,
    album,
    --
    CONSTRAINT pk_em_album
        PRIMARY KEY (album),
    --
    CONSTRAINT fk_em_supfisico 
        FOREIGN KEY (supfisico) 
        REFERENCES supfisico (tipo_sup)
        ON DELETE CASCADE,
    --
    CONSTRAINT fk_em_album
        FOREIGN KEY (album) 
        REFERENCES album (mbid)
        ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE versao (
    ean         VARCHAR (13),
    designacao  VARCHAR (30)    CONSTRAINT nn_versao_designacao NOT NULL,
    tabela_em,       
    --
    CONSTRAINT pk_versao 
        PRIMARY KEY (ean),
    --
    CONSTRAINT ck_versao_ean 
        CHECK (ean >= 0 AND REGEXP_LIKE(ean, '^[0-9]{13}$')),  -- RIA-12
    --
    CONSTRAINT fk_versao_em
        FOREIGN KEY (tabela_em)
        REFERENCES tabela_em (album)
        ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE utilizador (
  username                      VARCHAR (30),
  email                         VARCHAR (40)  CONSTRAINT nn_utilizador_email NOT NULL,
  palavra_passe                 VARCHAR (20)  CONSTRAINT nn_utilizador_palavra_passe NOT NULL,
  data_nascimento               DATE          CONSTRAINT nn_utilizador_data_nascimento NOT NULL,
--
  CONSTRAINT pk_utilizador
    PRIMARY KEY (username),
--
  CONSTRAINT ck_utilizador_username  
    CHECK (REGEXP_LIKE(username, '^[A-Za-z0-9]+$')),  -- RIA-14
--
  CONSTRAINT un_utilizador_email -- chave candidata
    UNIQUE (email) -- RIA 13
);
-------------------------------------------------------------------------------------------
CREATE TABLE lista_personalizada (
  utilizador,
  nome          VARCHAR (30) CONSTRAINT nn_lista_personalizada_nome NOT NULL,
--
  CONSTRAINT pk_lista_personalizada
    PRIMARY KEY (utilizador, nome),
--
  CONSTRAINT fk_lista_personalizada_uti
    FOREIGN KEY (utilizador)
    REFERENCES utilizador (username)
    ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE refere (
    utilizador,
    nome_lista,
    album,
    --
    CONSTRAINT pk_refere PRIMARY KEY (utilizador, nome_lista, album),
    --
    CONSTRAINT fk_refere_lista_personalizada
        FOREIGN KEY (utilizador, nome_lista)
        REFERENCES lista_personalizada (utilizador, nome)
        ON DELETE CASCADE,
    --
    CONSTRAINT fk_refere_album
        FOREIGN KEY (album)
        REFERENCES album (mbid)
        ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE artista(
    isni                VARCHAR (16),
    nome                VARCHAR (40)       CONSTRAINT artista_nome NOT NULL,
    ano_inicio          NUMBER (4)          CONSTRAINT artista_ano_inicio NOT NULL,
    tipo_artista        CHAR (10)           CONSTRAINT nn_artista_tipo NOT NULL,
    --
    CONSTRAINT pk_artista 
        PRIMARY KEY (isni),
    --
    CONSTRAINT ck_artista_isni
        CHECK (REGEXP_LIKE(isni, '^[0-9]{16}$')), -- RIA-07
    --
    CONSTRAINT ck_artista_ano_inicio 
        CHECK (ano_inicio >= 1900),
    --
    CONSTRAINT ck_artista_tipo_artista
        CHECK (LOWER(tipo_artista) IN ('solista','grupo')) -- RIA 10
);
-------------------------------------------------------------------------------------------
CREATE TABLE possui (
  utilizador,
  versao,
  data_de_adicao     DATE     CONSTRAINT nn_possui_data_de_adicao NOT NULL,
--
  CONSTRAINT pk_possui
    PRIMARY KEY (utilizador, versao),
--
  CONSTRAINT fk_possui_utilizador
    FOREIGN KEY (utilizador)
    REFERENCES utilizador (username),
--
  CONSTRAINT fk_possui_versao
    FOREIGN KEY (versao)
    REFERENCES versao (ean)
);
-------------------------------------------------------------------------------------------
CREATE TABLE favorito (
  utilizador,
  artista,
--
  CONSTRAINT pk_favorito
    PRIMARY KEY (utilizador),
--
  CONSTRAINT fk_favorito_utilizador
    FOREIGN KEY (utilizador)
    REFERENCES utilizador (username),
--
  CONSTRAINT fk_favorito_artista
    FOREIGN KEY (artista)
    REFERENCES artista (isni)
);
-------------------------------------------------------------------------------------------
CREATE TABLE solista (
    artista,
    data_nascimento DATE CONSTRAINT nn_solista_data_nascimento NOT NULL,
    --
    CONSTRAINT pk_solista 
        PRIMARY KEY (artista),
    --
    CONSTRAINT fk_solista_artista 
        FOREIGN KEY (artista) 
        REFERENCES artista (isni) 
        ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE grupo (
    artista,
    data_formacao DATE CONSTRAINT nn_grupo_data_formacao NOT NULL,
    --
    CONSTRAINT pk_grupo 
        PRIMARY KEY (artista),
    --
    CONSTRAINT fk_grupo_artista 
        FOREIGN KEY (artista) 
        REFERENCES artista (isni) 
        ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
CREATE TABLE interpretado (
    album,
    artista,
--
CONSTRAINT pk_interpretado
	PRIMARY KEY (album),
-- 
CONSTRAINT fk_interpretado_album
	FOREIGN KEY (album)
	REFERENCES album (mbid)
	ON DELETE CASCADE,
--
CONSTRAINT fk_interpretado_artista
	FOREIGN KEY (artista)
	REFERENCES artista (isni)
	ON DELETE CASCADE
);
-------------------------------------------------------------------------------------------
--Inserir supfisico
INSERT INTO supfisico (tipo_sup) VALUES ('CD');
INSERT INTO supfisico (tipo_sup) VALUES ('Vinil');
INSERT INTO supfisico (tipo_sup) VALUES ('Cassete');
-------------------------------------------------------------------------------------------
--Inserir album
INSERT INTO album (mbid, titulo, tipo, ano_de_lancamento) VALUES ('abcd1234abcd1234abcd1234abcd1234abcd', 'blonde', 'lp', 2016);
INSERT INTO album (mbid, titulo, tipo, ano_de_lancamento) VALUES ('1234abcd1234abcd1234abcd1234abcd1234', 'rodeo', 'lp', 2015);
-------------------------------------------------------------------------------------------
--Inserir tabela_em
INSERT INTO tabela_em (supfisico, album) VALUES ('CD', 'abcd1234abcd1234abcd1234abcd1234abcd');
INSERT INTO tabela_em (supfisico, album) VALUES ('Cassete', '1234abcd1234abcd1234abcd1234abcd1234');
-------------------------------------------------------------------------------------------
--Inserir versao
INSERT INTO versao (ean, designacao) VALUES ('1234567890123', 'Edicao Especial');
INSERT INTO versao (ean, designacao) VALUES ('9876543210098', 'Resmasterizada');
-------------------------------------------------------------------------------------------
--Inserir utilizador
INSERT INTO utilizador (username, email, palavra_passe, data_nascimento) VALUES ('joaosilva', 'joao.silva@gmail.com', 'abc123', DATE '2000-04-02');
INSERT INTO utilizador (username, email, palavra_passe, data_nascimento) VALUES ('maria99', 'maria.oliveira@gmail.com', 'xyz789', DATE '1999-09-02');
-------------------------------------------------------------------------------------------
--Inserir lista_personalizada
INSERT INTO lista_personalizada (utilizador, nome) VALUES ('joaosilva', 'Favoritos');
INSERT INTO lista_personalizada (utilizador, nome) VALUES ('maria99', 'Para Comprar');
-------------------------------------------------------------------------------------------
--Inserir refere
INSERT INTO refere (utilizador, nome_lista, album) VALUES ('joaosilva', 'Favoritos', 'abcd1234abcd1234abcd1234abcd1234abcd');
INSERT INTO refere (utilizador, nome_lista, album) VALUES ('maria99', 'Para Comprar', '1234abcd1234abcd1234abcd1234abcd1234');
-------------------------------------------------------------------------------------------
--Inserir artista
INSERT INTO artista (isni, nome, ano_inicio, tipo_artista) VALUES ('1234567890123456', 'Coldplay', 1996, 'grupo');
INSERT INTO artista (isni, nome, ano_inicio, tipo_artista) VALUES ('2345678901234567', 'Beyoncé', 1997, 'solista');
INSERT INTO artista (isni, nome, ano_inicio, tipo_artista) VALUES ('3456789012345678', 'The Beatles', 1960, 'grupo');
INSERT INTO artista (isni, nome, ano_inicio, tipo_artista) VALUES ('4672912381238819', 'Shawn Mendes', 1984, 'solista');
-------------------------------------------------------------------------------------------
--Inserir possui
INSERT INTO possui (utilizador, versao, data_de_adicao) VALUES ('joaosilva', '1234567890123', DATE '2021-03-10');
INSERT INTO possui (utilizador, versao, data_de_adicao) VALUES ('maria99', '9876543210098', DATE '2022-02-25');
-------------------------------------------------------------------------------------------
--Inserir favorito
INSERT INTO favorito (utilizador, artista) VALUES ('joaosilva', '1234567890123456');
INSERT INTO favorito (utilizador, artista) VALUES ('maria99', '2345678901234567');
-------------------------------------------------------------------------------------------
--Inserir solista
INSERT INTO solista (artista, data_nascimento) VALUES ('2345678901234567', DATE '1981-09-04');
INSERT INTO solista (artista, data_nascimento) VALUES ('4672912381238819', DATE '2003-11-23');
-------------------------------------------------------------------------------------------
--Inserir grupo
INSERT INTO grupo (artista, data_formacao) VALUES ('1234567890123456', DATE '1996-01-01');
INSERT INTO grupo (artista, data_formacao) VALUES ('3456789012345678', DATE '1960-01-01');
-------------------------------------------------------------------------------------------
--Inserir interpretado
INSERT INTO interpretado (album, artista) VALUES ('abcd1234abcd1234abcd1234abcd1234abcd', '1234567890123456');
INSERT INTO interpretado (album, artista) VALUES ('1234abcd1234abcd1234abcd1234abcd1234', '1234567890123456');