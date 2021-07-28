<!-- title:turnup readme -->    
<!-- style:./default.css -->			
<!-- config:term-link-in-header -->			
<!-- config:write-comment -->			
<!-- config:header-numbering 2 4 -->			


# README - turnup

　この文書は、**turnup** のためのマニュアル文書です。
    

## Table of contents

<!-- embed:toc 2 4 -->
　

## turnup とは

　turnup は、Markdown 書法で作成したテキストファイルから HTML を出力するツールです
{{fn:turnupという名前は、mark down を mark up にひっくり返す(turn upする)という意味で付けられました。}}。
このマニュアル文書自体、turnup で作成されています。


## 用語

*[GFM]: GitHub Flavored Markdown というものの略語だそうです。turnup は可能な範囲で
サポートするように心掛けています。

*[Markdown]: マークダウンと読みます。HTML の Mark up をしないで済むという意図なんで
しょうけど、命名にセンスが‥‥‥ね。

*[HTML]: Hyper text markup language の略語です。


## 使い方

　入力となるファイルの名前をパラメータとして起動してください。生成された HTML は標準出力
に書き出されるので、必要に応じてリダイレクトしてください。

~~~
  turnup  input.md  >  output.htm
~~~

　入出力ファイルのエンコーディングは utf-8 で固定となっています。それ以外のエンコーディング
への対応は現状では予定されていません。


## 記法

　turnup の入力書式は、基本的には一般的な Markdown 書法と同じですが、いくつかの拡張
機能{{fn:`GitHub Flavored Markdown` とか色々あるみたいで、何が標準なのか良くわかりませんが。}}
を備えています。


### 見出し

　行頭から１〜６個の # を記述し、続けて（半角）スペースをひとつ書いた行は見出し行になります。
後続する文字列が見出しのタイトルとなります。# の数に応じて h1 〜 h6 になります。

~~~
 # header1
 ## header2
 ### header3
 #### header4
 ##### header5
 ###### header6
~~~

　[拡張機能](#拡張機能)により、[見出しに番号を自動付与](#見出しの自動ナンバリング)したり、
[見出しから目次を生成](#目次の生成)したりできます。

### 箇条書きリスト

　-（ハイフン）、+（プラス）、または *（アスタリスク）にスペースを後続させると、箇条書きリストになります。
ネストさせたい場合は、行頭にタブ文字を使用してください。

~~~
- List A
+ List B
	* List B - 1
	- List B - 2
		+ List B - 2 - 1
		* List B - 2 - 2
- List C
~~~

　上記は、以下のように出力されます。

- List A
+ List B
	* List B - 1
	- List B - 2
		+ List B - 2 - 1
		* List B - 2 - 2
- List C


　また、- などの先頭記号とスペースに続けて [ ] や [X] を記述すると、チェックボックス付きリストになります。

~~~
- Task list
	- [X] completed task
		* point 1
		* point 2
	- [ ] not completed task
~~~

　上記は、以下のように出力されます。

- Task list
	- [X] completed task
		* point 1
		* point 2
	- [ ] not completed task



### 番号付きリスト

　数字にピリオドとスペースを後続させると、番号付きリストになります。ネストさせたい場合は、
行頭にタブ文字を使用してください。

~~~
1. List A
1. List B
	1. List B - 1
	1. List B - 2
		1. List B - 2 - 1
1. List C
~~~

　上記は、以下のように出力されます。

1. List A
1. List B
	1. List B - 1
	1. List B - 2
		1. List B - 2 - 1
1. List C

### preブロック

　バッククォート3つ、またはチルダ3つだけからなる行で囲まれたブロックは <pre> ブロックとなります。
{{fn:スペースまたはタブで始まる行を pre ブロックとする仕様は採用していません}}

```
~~~
int main( void ) {
    std::cout << "hello world." << std::endl;
    return 0;
}
~~~
```

　上記は、以下のように出力されます。

~~~
int main( void ) {
    std::cout << "hello world." << std::endl;
    return 0;
}
~~~

　以下のように、preブロックを開始する行に言語などの名前をつけることでコードハイライトをする
機能もあります。これは turnup では**フィルタ機能**と呼ばれ、コードハイライトに限らずブロック
の中身に様々な変換をかける仕組みとして位置付けられています。

```
~~~c++
int main( void ) {
    std::cout << "hello world." << std::endl;
    return 0;
}
~~~
```

　しかし、turnup はこのフィルタ機能を**まだ実装していません**
{{fn:diffくらいなら難易度も高くないので実装したいのですが、まだリソースが確保できてません}}。
しかし、拡張機能として外部プログラムを用いたフィルタリングが可能となっています。
詳しくは、[](#外部フィルタの登録)を参照してください。

### テーブル

　以下のような記述でテーブルを作成できます。１行目は常にヘッダ（th タグ）であり、
各列の中央揃え、右寄せなどを制御したい場合には ２行目に :---: や ---: を記述します。
{{fn:この２行目の記述は省略できます。その場合、すべての列がデフォルトの左寄せになります。}}

~~~
	| header A | header B | header C |	
	| :------- | :------: | -------: |	
	|     0    |     1    |    2     |	
	|     3    |     4    |    5     |	
	|     6    |     7    |    8     |	
~~~

　上記は、以下のように出力されます。

	| header A | header B | header C |	
	| :------- | :------: | -------: |	
	|     0    |     1    |    2     |	
	|     3    |     4    |    5     |	
	|     6    |     7    |    8     |	


### 引用

　行頭を > で始めると引用になります。複数の > 連続させると多段の引用になります。
{{fn:現在の実装では > は行頭から始める必要があり、また複数の > の間に空白を入れることはできません。これは課題として将来修正される可能性があります。}}

~~~
> lv.1 - 1
> lv.1 - 2
>> lv.2 - 1
>> lv.2 - 2
>>> lv.3
> lv.1
>>>> lv.4
>> lv.2
~~~

　上記は、以下のように出力されます。

> lv.1 - 1
> lv.1 - 2
>> lv.2 - 1
>> lv.2 - 2
>>> lv.3
> lv.1
>>>> lv.4
>> lv.2



### 定義リスト

　以下のような記述により、定義リストを作成できます。turnup では、これによって定義された
用語は[自動リンク](#定義リストへの自動リンク)の対象となります。

~~~
  *[用語]: 用語の定義をここに記述します。
  複数行に渡る定義も記述できます。空行で終わります。

~~~

　上記は、以下のように出力されます。

<raw_html>
<dl>
  <dt>用語</dt>
  <dd>用語の定義をここに記述します。複数行に渡る定義も記述できます。空行で終わります。</dd>
</dl>
</raw_html>

### コメント

　turnup は、以下のような HTML 形式のコメントをコメントとして扱います。つまり、出力
される HTML の内容には含まれません。

<raw_html>
<pre>
  &lt;!-- comment --&gt;

  &lt;!--
    multi
    line
    comment
  --&gt;
</pre>
</raw_html>

　ただし、この挙動は変更することができます。[write-comment設定](#コメントの出力)を参照して
ください。



### 強調

　1 〜 3 個の連続するアスタリスクまたはアンダースコアで括ることにより、文字列を強調することが
できます。どのように表示されるのかは、スタイルシート次第です。

* １つだと、em タグによる強調になります。`*これ*`が、*これ* になります。
* ２つだと、strong タグによる強調になります。`**これ**`が、**これ** になります。
* ３つだと、em と strong の併用になります。`***これ***`が、***これ*** になります。

### 取消し線

　２つのチルダ ~~ で括ることで、文字列に取消線を引くことができます。
`~~この部分は取消し~~`が、~~この部分は取消し~~になります。

### マーカー

　== を使って括ることで、マーカーを利用することができます。
スタイルシート次第ですが、`==例えばこんなふう==`が、==例えばこんなふう==になります。


### 上付きと下付き

　単一の ^ および ~ で括ると、文字列を上付きと下付きにできます。
たとえば `H~2~O` は H~2~O に、`2^10^ = 1024` は 2^10^ = 1024 になります。


### リンク

　文章中で以下のような書き方をすることで、URL へのリンクを貼ることができます。
{{fn:GFM だと URL を単純に書いただけでもリンクになるようですが、そこまではしてません。}}

```
  [テキスト](URL)
```

　たとえば、`[Google](https://www.google.co.jp/)` は [Google](https://www.google.co.jp/) と
なります。テキスト部分を省略すると、URL がそのまま表示されます。つまり、
`[](https://www.google.co.jp/)` は [](https://www.google.co.jp/) となります。

　拡張機能として、ページ内の見出しや図表タイトルへのリンクを作成することもできます。[$@ 節](#ページ内へのリンク)
を参照してください。


### コード

　文章中では、以下のようにバッククォートで括ることで、囲まれた部分をコードとして表示させられます。
この内部にある文字列は、用語の自動リンクやその他の文字装飾は実行されません。

~~~
  `code`
~~~

### 水平線

　行頭からハイフン、等号、アンダースコア、アスタリスクを３個以上を記述すると水平線として出力
されます。文字種は混在させないでください。

### 画像の挿入

　画像を表示させるには、以下のように記述します。独立した行として記述することもできますし、
文章中にインラインで挿入させることもできます。

~~~
  ![代替テキスト](画像URL)
~~~

　上記は、以下のように展開されます。なお、代替テキストは省略できます。

~~~
  <img src='画像URL' alt='代替テキスト' />
~~~

## 拡張機能

　turnup には、いくつかの拡張機能があります。一般的な Markdown の書法からは逸脱しますが、
文書作成に必要と判断して追加しているものです。

### 文書タイトルとスタイルシートの指定

　データファイルの冒頭に以下のようなコメントを記述することで、出力される HTML ファイルのヘッダに
設定するタイトルとスタイルシートを指定することができます。

~~~
  <!-- title:Document title -->
  <!-- style:./default.css -->
~~~

　このうち、スタイルシートの指定は設定の影響を受けます。詳細は
[embed-stylesheet設定](#スタイルシートの埋め込み)を参照してください。

### 図と表のタイトル

　図と表には、専用のタイトルをつけることができます。図（大抵は画像を表示させるという意味ですが）の
場合、以下のように行頭に `Figure.` を、それに続いてタイトルを書きます。

~~~
![](some-figure.png)
Figure. title of figure
~~~

　表の場合は行頭に `Table.` を、それに続いてタイトルを書きます。

~~~
Table. title of table
| header1 | header2 |
| :------ | :-----: |
|    0    |    1    |
|    2    |    3    |
~~~

　この方法でタイトルをつけておけば、同じく拡張機能の[図表一覧の生成](#図表一覧の生成)機能で一覧を
生成することができます。

　見出しと同様に、図表のタイトルには自動的に番号付けを行なうことができます。詳細は
「[](#図表のナンバリング単位)」を参照してください。


### ページ内へのリンク

　リンクの書法を拡張することで、ページ内の[$$](#見出し)や[図表タイトル](#図と表のタイトル)へのリンクを作成
することができます。

　ページ内の見出しへのリンクを作成する場合、URL 部分を # に続けて見出しの名前にしてください。
たとえば、`[目次](#Table of contents)` は[目次](#Table of contents)となります。
テキスト部分を省略した場合のルールは標準的な URL リンクと同じですが、
[見出しの自動ナンバリング](#見出しの自動ナンバリング)を設定している場合、番号付けされた見出し名が
リンクのテキストとなることに注意してください。つまり、`[](#Table of contents)` は
[](#Table of contents)となります。

　図表タイトルへのリンクを作成するには、（ちょっと無様ですが）以下のように F#, T# に続けて
タイトルを記述します。それ以外のルールは見出しへのリンクと同じです。

```
  [テキスト](F#図のタイトル)
  [テキスト](T#表のタイトル)
```

　さらに、テキスト部分では `$@`, `$$` を使うことができます。これらは、以下のように展開されます。

* `$@` : 自動ナンバリングによって付与されるプレフィクス
* `$$` : 見出しや図表タイトルの文字列


### 生の HTML の出力

　以下のように記述することで、生の HTML をそのまま出力することができます。

~~~
  <raw_html>
      :
      :
  </raw_html>
~~~

### 脚注

　文章中に脚注を設定し、一箇所にまとめて表示させることができます。開始を示す `{{fn:` と
終了を示す `}}` は同一の行に記述する必要があることに注意してください。

~~~
  {{fn:これが脚注の内容です。}}
~~~

　脚注をまとめて表示するには、脚注を展開したい場所（通常は登場文書の末尾でしょう）に
以下の行を記述します。これにより、その場所に文書内のすべての脚注が記載順で一覧されます。

~~~
  <!-- embed:footnotes -->
~~~

　このマニュアル文書でも、いくつかの脚注を（サンプルを兼ねて）使用しています。文書末尾を
参照してください。



### 目次の生成

　見出し情報から目次を生成することができます。以下のように記述すると、その場所に階層化
されたリストの形式で目次が作成されます。

```
  <!-- embed:toc -->
```

　上記の指定では、見出しレベルの１から６までが全て目次化の対象になります。この対象範囲を
絞るには、以下のように記述します。

```
  <!-- embed:toc 2 4 -->
```

　この例では、見出しレベル２〜４までが目次化の対象となります。



### 図表一覧の生成

　目次と同様、図と表の一覧を生成することができます。以下のように記述すると、その場所に図の
一覧が作成されます。

```
  <!-- embed:figure-list -->
```

　表の一覧の場合は以下です。

```
  <!-- embed:table-list -->
```

　これによって生成される一覧でも、[図表タイトル](#図と表のタイトル)と同じ設定でナンバリングが行なわれます。
詳細は「[](#図表のナンバリング単位)」を参照してください。


### 定義リストへの自動リンク

　[定義リスト](#定義リスト)をつかって用語を定義すると、文章中に登場したその用語は自動的に
定義箇所へのリンクになります。このマニュアル文書では、「[](#用語)」というセクションで
（サンプルを兼ねて）いくつかの用語を定義しているので、たとえば Markdown という用語は
自動的にリンクされます。
{{fn:この自動リンクにおいて、大文字小文字が異なる表記は対象になりません。この挙動を変更するオプションは今のところ、ありません。}}

　この拡張機能に関与する設定として、[見出しにおける用語の自動リンク](#見出しにおける用語の自動リンク)が
あります。



### 外部フィルタの登録

　[preブロック](#preブロック) で指定するフィルタとして、外部プログラムを追加することができます。
たとえば、diff というフィルタを指定した場合に highlight プログラムを通した結果を表示させたければ
以下のように指定します。

```
  <!-- filter:diff = highlight diff %in %out -->
```

　ここで `<!-- filter:` の直後から `=` まで（ここでは `diff` ）がフィルタ名、`=` から 最後
の `-->` の手前まで（ここでは `highlight diff %in %out` ）が実行するフィルタプログラムです。
%in と %out は入力ファイル名と出力ファイル名のプレースホルダで、実際に使用されるファイル名は 
turnnup が都度決定します。

　上記のように定義されていると、turnup はフィルタ名として diff が指定された preブロックを
見つけた場合に、以下の要領で処理を行ないます。

* preブロックの中身を一時ファイルに保存する
* フィルタプログラムの定義に従い、`highlight diff %in %out` を実行する
	* この時、preブロックの中身を保存した一時ファイル名で %in を置き換える
	* 同様に、%out を別の一時ファイル名で置き換える
* %out に指定した一時ファイルの内容を読み込み、preブロックのフィルタ結果として出力する

　外部フィルタは turnup が提供するフィルタ機能よりも優先度が高いことに注意してください。同じ名前の
フィルタがある場合、外部フィルタが優先されます（turnup はまだフィルタを実装していないので無意味な
注意事項ですが、仕様上はそうなるという話です）。

　　

## 設定

### 見出しにおける用語の自動リンク

　[定義リストへの自動リンク](#定義リストへの自動リンク)機能は、デフォルトでは見出し文字列
には適用されません。これは見出し内にリンクを生成するとうるさくなることや、見出しに存在する
文字列は通常そのセクションの本文にも存在することが理由です。

　この挙動を変えたい場合、以下の行を文書内に記載してください。見出し文字列内でも用語の自動
リンクが行なわれるようになります。

```
  <!-- config:term-link-in-header -->			
```

### 見出しの自動ナンバリング

　見出しに自動的に番号付けを行なうことができます。この機能を有効化するには、以下の設定を
文書内に記載します。

```
  <!-- config:header-numbering -->			
```

　これにより、1.1.1. 形式で見出しに自動的に番号が付与されます{{fn:番号付けの書式は今のところこれだけで、選択はできません。}}。
この番号を付与する見出しのレベルを限定したい場合、以下のように記載します。この例では、
見出しレベル２〜４の合計３レベルで採番されます。
この場合、見出しレベル1、および 5 以降には番号付けは適用されません。

```
  <!-- config:header-numbering 2 4 -->			
```


### 図表のナンバリング単位

　[図表タイトル](#図と表のタイトル)、および[図表一覧生成](#図表一覧の生成)に適用される番号付けの
規則を設定することができます。これには、以下のように記述します。この設定を省略した場合のデフォルト
値は 0 です。

```
  <!-- config:entity-numbering-depth 1 -->
```

　0 が指定された場合、図と表それぞれについて、文書全体を通して 1 から始まる連番が付与されます。
つまり、図であれば Figure.1 xxxxx、Figure.2 yyyyy といった要領です（表の場合は Table.1 zzzzz）
となります。

　1 以上を指定した場合、「[](#見出しの自動ナンバリング)」で指定した番号付け対象の見出しのうち、
先頭から指定した数のレベルを単位として 1 から始まる番号付けを行ないます。こう書くとわかりにくい
ですが、たとえば以下のようにした場合、

```
  <!-- config:header-numbering 2 4 -->			
  <!-- config:entity-numbering-depth 2 -->
```

まず、ナンバリングをする見出しはレベル 2,3,4 です。そして、図表については見出しレベルの 2,3 が一致
するものについて 1 から連番で番号付けをし、これらの見出しレベルが変化するとまた 1 からナンバリングが
始まります。なお、この場合は図であれば Figure.1.2-1 xxxxxx といった書式になります。



### スタイルシートの埋め込み

　[スタイルシートの指定](#文書タイトルとスタイルシートの指定)は、デフォルトでは
スタイルシートの参照が出力されるだけですが、以下の設定が文書内に記述されていると、
出力 HTML の内部にスタイルシートの内容を埋め込むようになります。

```
  <!-- config:embed-stylesheet -->			
```

　これによって、以下のように埋め込みが行なわれます。

~~~
<html>
<header>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<meta http-equiv="Content-Style-Type" content="text/css">
	<title>document title</title>
  <style>
  <!--
    :
    :
  -->
  </style>
</header>
~~~

　この設定を有効にしてスタイルシートの埋め込みを行なう場合、turnup の実行時点で
指定されたスタイルシートのファイルが存在していなければなりません（当たり前ですが）。



### コメントの出力

　[コメント](#コメント)は記述しても HTML には出力されませんが、以下の設定を文書内に
記述することで、この挙動を変えることができます。具体的には、HTML スタイルのコメントを
そのまま出力するようになります。
{{fn:それの何が嬉しいのかわからないかもしれませんが、作者にも良くわからない機能なので気にしないでください。おそらく、どのような設定で turnup が動作したのかわかるように、とかだと思います。}}

```
  <!-- config:write-comment -->			
```

　　

## 細かい話

* 文章中に記述できる各種の装飾、リンクなどは、行を跨ぐことはできないので注意してください。

* 他、何か思いついたら記載します。

　　

--------------------------------------------------------------------------------

<!-- embed:footnotes -->
