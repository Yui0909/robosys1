# robosys1
ロボットシステム学の課題で作成したオリジナルのデバイスドライバ。

## 概要
4つのLEDを消灯・点灯(点滅)させる。

## 準備するもの・動作環境
・Raspberry Pi 4 Model B  
・ブレッドボード  
・LED 4個  
・ジャンパー線　オス－オス 4本  
・ジャンパー線　オス－メス 5本  
・抵抗 220Ω 4個  
・Ubuntu 18.04　　

## 配線の様子
今回はGPIO22,23,24,25を使用。  
  
<img src="https://user-images.githubusercontent.com/72923980/101601938-a70e9200-3a40-11eb-8e78-276d45d56692.jpg" width="300">

## 動作内容
0 : 全て消灯  
1 : 4つ同時に点滅  
2 : 2つずつ点滅  
3 : 4つがそれぞれ早く点滅(当たり演出)  

## 実行手順
リポジトリをクローンする  
~~~
$ git clone https://github.com/Yui0909/robosys1.git
~~~  
ディレクトリへ移動  
~~~
$ cd robosys1
~~~  
Makefileを実行してカーネルモジュールを作成  
~~~
$ make
~~~  
カーネルモジュールをインストール
~~~
$ sudo insmod myled.ko
~~~  
パーミッションを変更  
~~~
$ sudo chmod 666 /dev/myled0
~~~  
実行  
例）0の動作を行う場合  
~~~
$ echo 0 > /dev/myled0
~~~   
カーネルモジュールをアンインストール  
~~~
$ sudo rmmod myled
~~~  

## デモンストレーション
https://youtu.be/6n_74eemWbU
