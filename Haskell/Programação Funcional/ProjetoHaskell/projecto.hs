-- João Pereira 63779
-- Isaura Cid 63761

import Test.QuickCheck
import System.Environment (getArgs)
import Network
import VectorMatrix
import Training

sameStructure :: Network -> Network -> Bool
sameStructure (Network [])(Network[]) = True
sameStructure (Network (Layer wIn bIn : restoIn)) (Network (Layer wOut bOut : restoOut)) = 
  length wIn == length wOut && map length wIn == map length wOut &&
  length bIn == length bOut && sameStructure (Network restoIn) (Network restoOut)
sameStructure _ _ = False

--propriedades
prop_sameStructure :: Double -> Property
prop_sameStructure lr =
    lr > 0 && lr < 1 ==>
    let net  = buildNetwork 2 [4,1] (repeat 0.1)
        net' = backPropagation lr [1,0] [1] net
    in sameStructure net net'

prop_transpose :: [[Int]] -> Property
prop_transpose xs =
    not (null xs) && not (null (head xs)) ==>
    let n = length (head xs)
        matrix = filter (\r -> length r == n) xs
    in not (null matrix) ==>
       transpose (transpose matrix) == matrix

prop_sigmoid :: Double -> Bool
prop_sigmoid x = let s = sigmoid x in s >= 0 && s <= 1

runTests :: IO()
runTests = do
  quickCheck prop_transpose
  quickCheck prop_sigmoid
  quickCheck prop_sameStructure

--main

main :: IO ()
main = do
  args <- getArgs

  case args of
    ["--train", file]   -> trainMode file

    ["--predict", file] -> predictMode file

    ["--test"]          -> runTests

    _                   -> putStrLn "Uso: --train ficheiro | --predict ficheiro | --test"