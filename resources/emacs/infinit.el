;; An example of an emacs configuration file for generating some of the
;; coding style elements such as sections and boxes.

;; resection
(global-set-key [(control c)(s)] 'resection-generate)
(load-file "~/.emacs.d/resection.el")

;; rebox
(setq rebox-default-style 223)
(global-set-key [(control c)(b)] 'rebox-comment)
(load-file "~/.emacs.d/rebox.el")
