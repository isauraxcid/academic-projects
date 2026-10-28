-- João Pereira 63779
-- Isaura Cid 63761

module VectorMatrix where

-- | Divide uma lista em grupos de tamanho n.
-- Exemplo: chunksOf 2 [1,2,3,4,5] == [[1,2],[3,4],[5]]
chunksOf :: Int -> [a] -> [[a]]
chunksOf n w = [take n (drop i w) | i <- [0, n .. length w - 1]]

-- | Função de ativação sigmoid.
-- Exemplo: sigmoid 0 == 0.5
sigmoid :: Double -> Double
sigmoid x = 1 / (1 + exp (-x))

-- | Derivada da sigmoid (em termos da saída, não da entrada).
-- Exemplo: sigmoid' 0.5 == 0.25
sigmoid' :: Double -> Double
sigmoid' y =  y * (1 - y)

-- | Transpõe uma matriz (troca linhas por colunas).
-- Exemplo: transpose [[1,2],[3,4],[5,6]] ==[[1,3,5],[2,4,6]]
transpose :: [[a]] -> [[a]]
transpose w = [ [row !! i | row <- w] | i <- [0..length (head w) - 1]]

-- | Multiplica uma matriz por um vetor: y[i] = sum(W[i][j] * x[j]).
-- Exemplo: multMatrix [[1,0],[0,1]] [3,4] == [3,4]
multMatrix :: [[Double]] -> [Double] -> [Double]
multMatrix w x = [sum [a*b | (a,b) <- zip row x] | row <- w]

-- | Soma ponto-a-ponto de dois vetores.
-- Exemplo: somaVectorial [1,2] [3,4] == [4,6]
somaVectorial :: [Double] -> [Double] -> [Double]
somaVectorial v1 v2 = [x + y | (x,y) <- zip v1 v2]
