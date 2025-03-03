% adsrprt.cls, LaTeX class for ADSinC reports
% last modified: Feb 24, 2025.
% author: Aron Hardeman (a.hardeman@rug.nl), but
%   the \lstset{...} part and the pseudocode macros
%   are reused and modified code from Steffen Frey
%   and former ADSinC course staff.

%%
%%%%
%%%%%%
%%%%%%%%
%%%%%%%%%%
%%%%%%%%%%%%
%%%%%%%%%%%%%%
%--------------------------------------------%
% NOTE TO STUDENTS: DO NOT CHANGE THIS FILE, %
%   BUT CHANGE THE FILE report.tex INSTEAD!  %
%--------------------------------------------%
%%%%%%%%%%%%%%
%%%%%%%%%%%%
%%%%%%%%%%
%%%%%%%%
%%%%%%
%%%%
%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{adsrprt}[2025/02/22 Template for ADS reports]

\PassOptionsToClass{11pt}{article}
\ProcessOptions\relax
\LoadClass{article}

% colors
\newcommand\subtitlecolor{teal!70!black}
\newcommand\sectbgcolor{green!40}
\newcommand\secttextcolor{black}
\newcommand\sectshadowcolor{gray!60}
\newcommand\fancyheaderframecolor{teal!30}
\newcommand\fancyheaderbgcolor{green!4}
\newcommand\fancyheadershadowcolor{red!30!gray}
\newcommand\tcexamplehorizbordercolor{gray!70}
\newcommand\tcexampleoutervertbordercolor{orange!70}
\newcommand\tcexampleinnervertbordercolor{yellow!70}
\newcommand\tcexamplebgcolor{yellow!10}
\newcommand\labelitemicolor{orange}
\newcommand\labelitemiicolor{orange!40!yellow}
\newcommand\labelitemiiicolor{orange}
\newcommand\labelitemivcolor{orange!40!yellow}

% lengths
\newlength\titletop\setlength\titletop{5.2pt}
\newlength\titlemiddle\setlength\titlemiddle{1.95em}
\newlength\titlebottom\setlength\titlebottom{13mm}

% misc
\RequirePackage{xcolor}
\RequirePackage[english]{babel}
\RequirePackage{amsmath}
\RequirePackage{amssymb}
\RequirePackage{microtype}
\RequirePackage{parskip}
\RequirePackage{listings}
\RequirePackage[skins,listings]{tcolorbox}
\RequirePackage[colorlinks=true]{hyperref}

% page geometry
\RequirePackage{geometry}
\geometry{a4paper,left=20mm,right=20mm,top=30mm,bottom=25mm}

% font
\RequirePackage[T1]{fontenc}
\RequirePackage{mlmodern}

% we use \title, \subtitle, \headertitle and \headersubtitle,
% instead of \title, \author and \date.
\def\title#1{\gdef\@title{#1}}
\def\@title{\@latex@error{No \noexpand\title is given}\@ehc}
\def\subtitle#1{\gdef\@subtitle{#1}}
\def\@subtitle{\@latex@error{No \noexpand\subtitle is given}\@ehc}
\def\headertitle#1{\gdef\@headertitle{#1}}
\def\@headertitle{\@latex@error{No \noexpand\headertitle is given}\@ehc}
\def\headersubtitle#1{\gdef\@headersubtitle{#1}}
\def\@headersubtitle{\@latex@error{No \noexpand\headersubtitle is given}\@ehc}

% fancy \maketitle
\def\maketitle{%
  \null\vspace\titletop
  \begin{center}
    \centering
    \huge\ttfamily
    \@title\\
    \vspace\titlemiddle
    \color{\subtitlecolor}\Large \@subtitle
  \end{center}
  \vspace\titlebottom}

% fancy header
\RequirePackage{lastpage}
\RequirePackage{eso-pic}
\pagestyle{empty}
\AddToShipoutPicture{\put(\LenToUnit{10.2cm},\LenToUnit{\paperheight-2cm}){%
  \ifnum\value{page}=1%
  \else%
  \begin{tcolorbox}[
      enhanced,
      colframe=\fancyheaderframecolor,
      colback=\fancyheaderbgcolor,
      sharp corners,
      drop fuzzy shadow=\fancyheadershadowcolor]
    \ttfamily\bfseries \@headertitle\\
    \@headersubtitle\ (page \thepage\ of \pageref*{LastPage})
  \end{tcolorbox}%
  \fi}}

% redefinition of \section, \section*
\RequirePackage{needspace}
\def\section{\secdef\boxsect\sboxsectwr}
\def\boxsect[#1]#2{%
  \needspace{4cm}%
  \vspace{4mm}%
  \refstepcounter{section}%
  \sboxsect{\thesection. #2}%
  \addcontentsline{toc}{section}{\protect\numberline\thesection#2}}
\def\sboxsectwr#1{%
  \needspace{4cm}%
  \vspace{4mm}%
  \sboxsect{#1}}
\def\sboxsect#1{%
  \begin{tcolorbox}[%
      enhanced,%
      spread sidewards,%
      sharp corners,%
      colback=\sectbgcolor,%
      colframe=\sectbgcolor,%
      fuzzy shadow={0mm}{-1mm}{0mm}{0.1mm}{\sectshadowcolor}]%
    \hspace{14mm}\textcolor{\secttextcolor}{\Large\textbf{\texttt{#1}}}%
  \end{tcolorbox}%
  \vspace{2mm}}

% {itemize} bullets
\renewcommand\labelitemi{$\textcolor{\labelitemicolor}{\pmb\rightarrow}$}
\renewcommand\labelitemii{$\textcolor{\labelitemiicolor}{\pmb\rightarrow}$}
\renewcommand\labelitemiii{$\textcolor{\labelitemiiicolor}{\pmb\rightarrow}$}
\renewcommand\labelitemiv{$\textcolor{\labelitemivcolor}{\pmb\rightarrow}$}

% for typesetting pseudocode:
\newcommand*\Algorithm{\textbf{algorithm}}
\newcommand*\Comment[1]{{\color{gray}/$\ast$~#1~$\ast$/}}
\newcommand*\Input{\textbf{input}}
\newcommand*\Result{\textbf{result}}
\newcommand*\If{\textbf{if}}
\newcommand*\Else{\textbf{else}}
\newcommand*\Then{\textbf{then}}
\newcommand*\While{\textbf{while}}
\newcommand*\For{\textbf{for}}
\newcommand*\Forall{\textbf{forall}}
\newcommand*\To{\textbf{to}}
\newcommand*\Do{\textbf{Do}}
\newcommand*\becomes{$\leftarrow$}

% listing style
\lstset{
  language=C,
  tabsize=2,
  showstringspaces=false,
  breaklines=true,
  basicstyle=\ttfamily,
  keywordstyle=\color[rgb]{0.1,0.3,0.7}\ttfamily,
  stringstyle=\color[rgb]{0.7,0.1,0.3}\ttfamily,
  commentstyle=\color[rgb]{0.3,0.4,0.3}\ttfamily,
  columns=fixed,
  backgroundcolor=\color[rgb]{0.95,0.95,0.95},
  frame=lines,
  framexleftmargin=5pt,
  xleftmargin=15pt,
  numbers = left,
  numberstyle = \footnotesize
}

