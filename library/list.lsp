;;
;; list.l : ISLisp 用リスト操作関数
;;
;;          Copyright (C) 2016 Makoto Hiroi
;;

;; 参照
(defun caar (xs) (car (car xs)))
(defun cadr (xs) (car (cdr xs)))
(defun cdar (xs) (cdr (car xs)))
(defun cddr (xs) (cdr (cdr xs)))

(defun first  (xs) (car xs))
(defun second (xs) (cadr xs))
(defun third  (xs) (elt xs 2))  ; (elt xs n) は列関数
(defun fourth (xs) (elt xs 3))
(defun fifth  (xs) (elt xs 4))

;; 末尾のセル
(defun last-pair (xs)
  (for ((xs xs (cdr xs)))
       ((null (cdr xs)) xs)))

;; 末尾の要素
(defun last (xs) (car (last-pair xs)))

;; 先頭から n 個の要素を取り出す
(defun take (xs n)
  (for ((n n (- n 1))
        (a nil (cons (car xs) a))
        (xs xs (cdr xs)))
       ((or (<= n 0) (null xs)) (nreverse a))))

;; 先頭から n 個の要素を取り除く
(defun drop (xs n)
  (for ((n n (- n 1))
        (xs xs (cdr xs)))
       ((or (<= n 0) (null xs)) xs)))

;; xs を反転して ys と連結する
(defun append-reverse (xs ys)
  (for ((xs xs (cdr xs)))
       ((null xs) ys)
       (setq ys (cons (car xs) ys))))

;;
;; リストの生成
;;
(defun iota (n m)
  (for ((m m (- m 1))
        (a nil))
       ((> n m) a)
       (setq a (cons m a))))
;;
;; 削除
;;

;; 重複要素を削除する
(defun remove-duplicate (xs)
  (for ((xs xs (cdr xs))
        (ys nil))
       ((null xs) (nreverse ys))
       (if (not (member (car xs) ys))
           (setq ys (cons (car xs) ys)))))

;; フィルター
(defun remove-if (f xs)
  (for ((xs xs (cdr xs))
        (a nil))
       ((null xs) (nreverse a))
       (if (not (funcall f (car xs)))
           (setq a (cons (car xs) a)))))

(defun remove (x xs)
  (remove-if (lambda (y) (eql x y)) xs))

;;
;; 畳み込み
;;
(defun fold-left (f a xs)
  (for ((ys xs (cdr ys))
        (acc a (funcall f acc (car ys))))
       ((null ys) acc)))

(defun fold-right (f a xs)
  (for ((ys (reverse xs) (cdr ys))
        (acc a (funcall f (car ys) acc)))
       ((null ys) acc)))

;;
;; 巡回
;;
(defun for-each (f xs)
  (for ((ys xs (cdr ys)))
       ((null ys))
       (funcall f (car ys))))

;;
;; 分割
;;
(defun partition (pred xs)
  (for ((xs xs (cdr xs))
        (ys nil)
        (zs nil))
       ((null xs) (cons (nreverse ys) (nreverse zs)))
       (if (funcall pred (car xs))
           (setq ys (cons (car xs) ys))
         (setq zs (cons (car xs) zs)))))

;;
;; 検索
;;
(defun find-if (pred xs)
  (block exit
    (for ((xs xs (cdr xs)))
         ((null xs))
         (if (funcall pred (car xs))
             (return-from exit (car xs))))))

(defun find (a xs)
  (find-if (lambda (x) (eql x a)) xs))

(defun position-if (pred xs)
  (block exit
    (for ((i 0 (+ i 1))
          (xs xs (cdr xs)))
         ((null xs) -1)
         (if (funcall pred (car xs))
             (return-from exit i)))))

(defun position (a xs)
  (position-if (lambda (x) (eql x a)) xs))

(defun count-if (pred xs)
  (for ((c 0)
        (xs xs (cdr xs)))
       ((null xs) c)
       (if (funcall pred (car xs)) (setq c (+ c 1)))))

(defun count (a xs)
  (count-if (lambda (x) (eql x a)) xs))

(defun any (pred &rest xs)
  (block exit
    (for ((xs xs (mapcar #'cdr xs)))
         ((member nil xs))
         (let ((ys (mapcar #'car xs)))
           (if (apply pred ys)
               (return-from exit t))))))

(defun every (pred &rest xs)
  (block exit
    (for ((xs xs (mapcar #'cdr xs)))
         ((member nil xs) t)
         (let ((ys (mapcar #'car xs)))
           (if (not (apply pred ys))
               (return-from exit nil))))))

;;
;; 集合演算
;;
(defun union (xs ys)
  (for ((xs xs (cdr xs))
        (zs nil))
       ((null xs) (append-reverse zs ys))
       (if (not (member (car xs) ys))
           (setq zs (cons (car xs) zs)))))

(defun intersection (xs ys)
  (for ((xs xs (cdr xs))
        (zs nil))
       ((null xs) (nreverse zs))
       (if (member (car xs) ys)
           (setq zs (cons (car xs) zs)))))

(defun difference (xs ys)
  (for ((xs xs (cdr xs))
        (zs nil))
       ((null xs) (nreverse zs))
       (if (not (member (car xs) ys))
           (setq zs (cons (car xs) zs)))))

(defun subsetp (xs ys)
  (block exit
    (for ((xs xs (cdr xs)))
         ((null xs) t)
         (if (not (member (car xs) ys))
             (return-from exit nil)))))