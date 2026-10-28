-- João Pereira 63779
-- Isaura Cid 63761

module Training where

import Network
import System.Random (newStdGen, randomRs)

-- | Treina a rede durante n iterações, com uma taxa de aprendizagem e percorrendo os exemplos ciclicamente.
training :: Int -> Double -> ([[Double]], [[Double]]) -> Network -> Network
training n lr (xs, ys) net = go n net (cycle (zip xs ys))
  where
    go 0 net _               = net
    go m net ((x, y) : rest) = go (m - 1) betterNet rest
      where 
        betterNet = backPropagation lr x y net

trainMode :: FilePath -> IO ()
trainMode filePath = do
  g <- newStdGen
  let ws = randomRs (-1.0, 1.0) g

  let net = buildNetwork 2 [4,1] ws

  let xorData   = [[0,0],[0,1],[1,0],[1,1]]
  let xorLabels = [[0],[1],[1],[0]]
  let trained = training 100000 0.1 (xorData, xorLabels) net

  writeFile filePath (show trained)
  putStrLn ("Pesos guardados em " ++ filePath)

  let predicts = map (\x -> last (forwardPass x trained)) xorData

  print (zip predicts xorLabels)

  putStrLn ("MSE: " ++ show (msePredictions predicts xorLabels))

predictMode :: FilePath -> IO ()
predictMode filePath = do
  content <- readFile filePath
  let net = read content :: Network

  putStrLn "Valor 1: "
  v1 <- readLn

  putStrLn "Valor 2: "
  v2 <- readLn

  let output = last (forwardPass [v1,v2] net)
  print $ map round output