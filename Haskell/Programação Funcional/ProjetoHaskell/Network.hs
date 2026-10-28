-- João Pereira 63779
-- Isaura Cid 63761

module Network where

import VectorMatrix

data Network = Network [Layer] deriving (Show,Read)
data Layer = Layer [[Double]] [Double] deriving (Show,Read)

-- | Constrói uma rede neuronal.
-- Exemplo: length (buildNetwork 2 [4,1] (repeat 0.1)) == 2
buildNetwork :: Int -> [Int] -> [Double] -> Network
buildNetwork inputSize layers ws = Network (go pairs ws)
  where
    pairs = zip layers (inputSize : init layers)
    go [] _ = []
    go ((n,m):rest) ws = layer : go rest rest_ws
      where
        weights = chunksOf m (take (n*m) ws)
        bias    = take n (drop (n*m) ws)
        layer   = Layer weights bias
        rest_ws = drop (n*m + n) ws

-- | Diferença entre previsão e alvo (por elemento).
-- Exemplo: outputError [0.9] [1.0] == [-0.1]
outputError :: [Double] -> [Double] -> [Double]
outputError = zipWith (-)

-- | Erro quadrático médio entre previsão e alvo.
-- Exemplo: mse [1.0] [0.0] == 1.0
mse :: [Double] -> [Double] -> Double
mse y y' = sum (map (^2) (outputError y y')) / fromIntegral (length y)

-- | MSE médio sobre um conjunto de previsões.
msePredictions :: [[Double]] -> [[Double]] -> Double
msePredictions ys ys' = sum (zipWith mse ys ys') / fromIntegral (length ys)

-- | Propaga a entrada pela rede e devolve todas as ativações,
-- da entrada até à saída: [entrada, act1, act2, ..., saída].
-- Exemplo: length (forwardPass [0,1] net) == length net + 1
forwardPass :: [Double] -> Network -> [[Double]]
forwardPass input (Network layers) = scanl propagate input layers
  where
    propagate x (Layer w b) = [sigmoid z | z <- somaVectorial (multMatrix w x) b]

-- | Executa um passo de backpropagation dado uma taxa de aprendizagem, o input e o output esperado para um exemplo concreto.
backPropagation :: Double -> [Double] -> [Double] -> Network -> Network
backPropagation learningRate input target (Network layers) = Network newLayers
  where
    activations = forwardPass input (Network layers)
    outputErr = outputError (last activations) target
    newLayers = reverse (go (reverse layers) (reverse (init activations)) outputErr)

    go [] _ _ = []
    go (Layer w b : rest) (a:as) err = Layer w' b' : go rest as err'
      where
        w' = [ [w_ij - learningRate * err_i * a_j | (w_ij, a_j) <- zip w_i a] | (w_i, err_i) <- zip w err]
        b' = [b_i - learningRate * err_i | (b_i, err_i) <- zip b err]
        err' = [sum [w_ij * err_i | (w_ij, err_i) <- zip w_i err] * sigmoid' a_j | (w_i, a_j) <- zip (transpose w) a]