
## 既知の問題点
<!-- autolink: [$$](#既知の問題点) -->

### 【解決済】リンクテキストの末尾に文字装飾があると正しく処理されない

<!-- collapse:begin -->
　この問題は 2021/09/12 に修正されました（まだバージョン番号の管理を始めていないので日付で記載しています）。

　リンクテキストの末尾に文字装飾があると正しく処理されない問題が確認されています。たとえば、
`[Table of __contents__](#Table of contents)` は `Table of __contents__` と表示
されてしまいます。

　~~内部実装に問題があることが確認できていますが、修正が少々面倒なため着手できてません。~~

<!-- collapse:end -->

${BLANK_PARAGRAPH}

### 【解決済】テーブルで colspan/rowspan を使うとアライメント指定がずれる

<!-- collapse:begin -->
アライメント指定行を任意の位置に複数記述可能とすることでユーザーデータでの対処を可能にする修正を \
行ないました。詳細はテーブルの説明を参照してください。

　テーブルにおいて列毎のアライメント指定と横／縦方向のセル結合を併用すると意図した
ようにアライメントが設定されない問題があります。これは、セルの処理においてセル結合の情報を
考慮に入れていないために発生します。たとえば、以下の例では水平方向の結合によってそれよりも
右側のセルで問題が生じています。

<!-- palette: 0 style="background: #fff0f0;" -->

```markdown
| column A | column B | column C |
|:---------|:--------:|---------:|
|left      |center    |right     |
|2> left              |right?    |
|left      |center    |right     |
```

| column A | column B | column C |
|:---------|:--------:|---------:|
|left      |center    |right     |
|2> left              |0] right? |
|left      |center    |right     |


　上記のような例では、問題は行に閉じているため対処は簡単だと思うかもしれません。しかし、
縦方向の結合では先行する行の影響を受けることがわかります。

```markdown
| column A | column B | column C |
|:---------|:--------:|---------:|
|2^ left   |center    |right     |
           |center?   |right?    |
|left      |center    |right     |
```

| column A | column B | column C |
|:---------|:--------:|---------:|
|2^ left   |center    |right     |
           |0] center?|0] right? |
|left      |center    |right     |


　さらに、これらのセル結合指定が複合した場合、プログラムとして正し処理を行なうのは難しく
なります。この問題に対する対処は未定ですが、コードで頑張ることは避け、アライメント指定行を
任意の位置に複数記述可能とすることでユーザーデータでの対処を可能にする方向で検討中です。

<!-- collapse:end -->


### 【解決済】条件分岐でファイル存在チェックをしても include がエラーになる

<!-- collapse:begin -->
　この問題は version 0.804 で修正されました。

　[$$](#内部的な処理順序) で説明している通り、[$$](#別ファイルの include)はプリプロセス
に先行して実施されます。そのため、以下のように「ファイルが存在した場合だけ include する」
という意図で書かれた以下のコードは期待される動作をしませんでした（入力データのロード時点でエラー
終了してしまいます）。

```raw
<pre>
&lt!-- if: -f foo.txt --&gt
  &lt!-- include: foo.txt --&gt
&lt!-- endif --&gt
</pre>
```

　この問題を解決するために、include の際のエラー出力方式変更を実施しました。具体的には、
入力データのロード時点ではエラー終了とせず、include 箇所に [errorディレクティブ](#エラーと警告の出力)
を埋め込むようにしました。これにより、エラー処理が出力生成段階まで遅延されることになり、
条件分岐で適切に括られていれば意図した動作をする（エラーが回避できる）ことになります。

<!-- collapse:end -->


### 【解決済】C/C++ のコードハイライトで #if や #else が正しく処理されない

<!-- collapse:begin -->
　この問題は version 0.808 で修正されました。

　[C/C++ のコードハイライト](#フィルタ機能)に問題があり、 `#if` や `#else` が正しく
処理されません。先に `if` や `else` が正しく処理されてしまうバグがあり、以下のように
なってしまいます。

```raw
<pre class='hl'>
<span class="hl kwb">int</span> <span class="hl kwd">main</span><span class="hl opt">(</span> <span class="hl kwb">int</span> argc<span class="hl opt">,</span> <span class="hl kwb">char</span><span class="hl opt">*</span> argv<span class="hl opt">[] ) {</span>
#<span class="hl kwa">if 0</span>
    <span class="hl kwd">foo</span><span class="hl opt">();</span>
#<span class="hl kwa">else</span>
    <span class="hl kwd">bar</span><span class="hl opt">();</span>
#<span class="hl kwa">endif</span>
    <span class="hl kwa">return</span> <span class="hl num">0</span><span class="hl opt">;</span>
<span class="hl opt">}</span>
</pre>
```

　この問題を解決するために、キーワードの処理順序を変更しました。以下のように正しくハイ
ライトされるように修正済みです。

```C
int main( int argc, char* argv[] ) {
#if 0
    foo();
#else
    bar();
#endif
    return 0;
}
```

<!-- collapse:end -->

### 【解決済】リストの仕様が一般的な markdown と異なる

<!-- collapse:begin -->
　この問題は version 0.816 で修正されました。

　一般的な markdown では、リストや番号付きリストの処理はかなり柔軟に行なわれますが、
それと比較すると turnup のリスト処理は限定的でした。version 0.816 において、
一般的な markdown の処理にあわせる対処を実施しました。

<!-- collapse:end -->

### 【解決済】pre ブロックやコメントブロック内に # で始まる行があると見出し番号がズレる

<!-- collapse:begin -->
　この問題は version 0.823 で修正されました。

　見出しや図表タイトルは[予備スキャン](#内部的な処理順序)の段階で情報収集して拡張機能の利用に
備えるようになっていますが、この予備スキャンにおいて、pre ブロックやコメントブロックの考慮が
されていませんでした。つまり、pre ブロックなどの内部に見出しや図表タイトルと同じ記述の行が
あると、見出しや図表のナンバリングが乱れる結果となります。version 0.823 において、予備
スキャンがこれらの「スキャン対象外ブロック」を認識するように修正されました。

<!-- collapse:end -->

${BLANK_PARAGRAPH}

