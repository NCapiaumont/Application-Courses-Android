#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDrag>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QTableWidgetItem>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->showMaximized(); // met la taille de la ppli a fond
    this->ouvreBase();
    this->chargerProduits();
    this->chargerRayon();
    this->chargerPanier();
    this->chargerListe();
  //  this->chargerRayon();
    ui->stackedWidget->setCurrentIndex(0);
}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::ouvreBase()
{
    base=QSqlDatabase::addDatabase("QSQLITE");
    //nom fichier qui contient la base sqlite3
    base.setDatabaseName("listeCourses.db");
    //ouverture de la base de données
    base.open();
    //si la base ne s'est pas ouverte correctement
    if(base.isOpen() != true)
    {
            QMessageBox::critical(this, "Erreur critique", "Impossible d'ouvrir la base de données.");
    }
    else
    {
            this->verifBase();
    }
}
void MainWindow::verifBase()
{
    this->base.exec("select rayonNum, rayonLibelle, rayonOrdre form rayon");
    this->base.exec("select produitNum, produitLibelle, rayonNum from produit");
    if (this->base.lastError().number()>0)//si la requete renvoie une erreur
    {
        qDebug()<<"here "+this->base.lastError().driverText();
        this->base.exec("begin;"); //début transaction SQL
        qDebug()<<"création de la base de données...";

///     drop toutes les tables
        this->base.exec("drop table produit;");
        this->base.exec("drop table rayon");
///     create les tables
        //table rayon
        this->base.exec("create table rayon(rayonNum int primary key default 0, rayonLibelle varchar(50), rayonOrdre int)");
        //table produit
        this->base.exec("create table produit(produitNum int primary key default 0, produitLibelle varchar(50), rayonNum int not null REFERENCES rayon(idRayon), quantite int default 0, etat smallint default 0)");
///     insert le jeu d'essai (produits, rayons, contenu)
        //des rayons
        this->base.exec("insert into rayon values(0,'Electromenagé',0)");
        this->base.exec("insert into rayon values(1,'Fruits',1)");
        this->base.exec("insert into rayon values(2,'Légumes',2)");
        this->base.exec("insert into rayon values(3,'Boucherie',3)");
        this->base.exec("insert into rayon values(4,'Poissonnerie',4)");
        this->base.exec("insert into rayon values(5,'Crèmerie',5)");
        this->base.exec("insert into rayon values(6,'Boulangerie',6)");
        this->base.exec("insert into rayon values(7,'Surgelé',7)");
        this->base.exec("insert into rayon values(8,'Boissons',8)");
        this->base.exec("insert into rayon values(9,'Bricolage',9)");
        this->base.exec("insert into rayon values(10,'Multimédia',10)");
        this->base.exec("insert into rayon values(11,'Papeterie',11)");
        this->base.exec("insert into rayon values(12,'Autres',12)");
        //des produits
        this->base.exec("insert into produit values(0,'Robot',0,0)");
        //
        this->base.exec("insert into produit values(1,'Banane',1,2,1)");
        this->base.exec("insert into produit values(2,'Pomme',1,1,1)");
        this->base.exec("insert into produit values(3,'Orange',1,0,)");
        this->base.exec("insert into produit values(4,'Citron',1,0,0)");
        //
        this->base.exec("insert into produit values(5,'Souris',10,1,2)");
        this->base.exec("insert into produit values(6,'Clavier',10,0,0)");
        this->base.exec("insert into produit values(7,'CD/DVD',10,0,0)");
        this->base.exec("insert into produit values(8,'Webcam',10,1,2)");
        //
        this->base.exec("insert into produit values(9,'Marteau',9,0,0)");
        this->base.exec("insert into produit values(10,'Pince',9,1,1)");
        this->base.exec("insert into produit values(11,'Scie',9,0,0)");
        this->base.exec("insert into produit values(12,'Tournevis',9,1,1)");
        this->base.exec("commit;"); //fin transaction
    }
}
void MainWindow::fermeBase()
{
    this->base.close();
}
void MainWindow::chargerComboBoxCreerProduitChoisirCategorie()
{
    ui->comboBoxCreerProduitChoisirCategorie->clear();
    QSqlQuery req("select rayonNum, rayonLibelle, rayonOrdre from rayon");
    while(req.next())
    {
        ui->comboBoxCreerProduitChoisirCategorie->addItem(req.value(1).toString());
    }
}
void MainWindow::chargerRayon()
{
    //vide la widgetRayonsModification
    ui->listWidgetRayonsModification->clear();
    ui->listWidgetRayons->clear();
    ui->listWidgetEditerRayon->clear();
    //vide le vecteurOrdreRayon
    vecteurRayon.clear();
    QSqlQuery req("select rayonNum, rayonLibelle, rayonOrdre from rayon order by rayonOrdre");
    while(req.next())
    {
        ui->listWidgetRayonsModification->addItem(req.value(1).toString());
        vecteurRayon.push_back(req.value(0).toInt());
        ui->listWidgetRayons->addItem(req.value(1).toString());
        ui->listWidgetEditerRayon->addItem(req.value(1).toString());
    }
    this->chargerComboBoxCreerProduitChoisirCategorie();
    ui->pushButtonEditerRayonSupprimer->setEnabled(false);
    ui->pushButtonEditerRayonModifier->setEnabled(false);
    ui->lineEditEditerRayon->clear();
}
void MainWindow::chargerListe()
{
    ui->listWidgetMaListeAAcheter->clear();
    currentRayonAffiche="null";
    vecteurListeAAcheter.clear();
    QSqlQuery req("select produitLibelle, quantite, rayonNum, produitNum from produit natural join rayon where etat=1 order by rayonOrdre");
    while(req.next())
    {
        QLinearGradient linGrad;
        QSqlQuery reqRayon("select rayonLibelle from rayon where rayonNum="+req.value(2).toString());
        reqRayon.first();
        if(reqRayon.value(0).toString()!=currentRayonAffiche)
        {
            currentRayonAffiche=reqRayon.value(0).toString();
            QListWidgetItem *itemRayon1=new QListWidgetItem("- "+reqRayon.value(0).toString());
            itemRayon1->setFlags(Qt::NoItemFlags);
            ui->listWidgetMaListeAAcheter->addItem(itemRayon1);
            linGrad.setColorAt( 0, QColor(250,250,150));//couleur
            QBrush brush(linGrad);//definit une QBrush
            itemRayon1->setBackground(brush);
            itemRayon1->setForeground(QBrush(Qt::black));
            vecteurListeAAcheter.push_back(NULL);

        }
        QListWidgetItem *myItem1=new QListWidgetItem(req.value(0).toString()+" x"+req.value(1).toString()+" | Pas acheté");
        ui->listWidgetMaListeAAcheter->addItem(myItem1);
        vecteurListeAAcheter.push_back(req.value(3).toInt());
    }
}
void MainWindow::chargerPanier()
{
   //vide la listeWidgetMaListePanier
   ui->listWidgetMaListePanier->clear();
   //vide le vecteurPanier
   vecteurPanier.clear();
   QSqlQuery req("select produitLibelle, produitNum, quantite from produit where etat=2");
   while(req.next())
   {
       ui->listWidgetMaListePanier->addItem(req.value(0).toString()+" x"+req.value(2).toString()+" | Acheté");
       vecteurPanier.push_back(req.value(1).toInt());
   }
}
void MainWindow::chargerProduits()
{
    ui->listWidgetProduit->clear();
    ui->listWidgetModificationProduit->clear();
    vecteurIdProduit.clear();
    currentRayonAffiche="null";

    QSqlQuery req("select produitLibelle, produitNum, rayonNum from produit natural join rayon order by rayonOrdre");
    while(req.next())
    {
        QLinearGradient linGrad;
        QSqlQuery reqRayon("select rayonLibelle from rayon where rayonNum="+req.value(2).toString());
        reqRayon.first();
        if(reqRayon.value(0).toString()!=currentRayonAffiche)
        {
            currentRayonAffiche=reqRayon.value(0).toString();
            QListWidgetItem *itemRayon=new QListWidgetItem("- "+reqRayon.value(0).toString());
            QListWidgetItem *itemRayonM=new QListWidgetItem("- "+reqRayon.value(0).toString());
            itemRayon->setFlags(Qt::NoItemFlags);
            itemRayonM->setFlags(Qt::NoItemFlags);
            ui->listWidgetProduit->addItem(itemRayon);
            ui->listWidgetModificationProduit->addItem(itemRayonM);
            linGrad.setColorAt( 0, QColor(250,250,150));//couleur
            QBrush brush(linGrad);//definit une QBrush
            itemRayon->setBackground(brush);
            itemRayon->setForeground(QBrush(Qt::black));
            itemRayonM->setBackground(brush);
            itemRayonM->setForeground(QBrush(Qt::black));
            vecteurIdProduit.push_back(NULL);
        }
        QListWidgetItem *myItem=new QListWidgetItem(req.value(0).toString());
        ui->listWidgetProduit->addItem(myItem);
        ui->listWidgetModificationProduit->addItem(req.value(0).toString());
        vecteurIdProduit.push_back(req.value(1).toInt());
    }
    //passe la quantité a 1
    ui->spinBoxProduitsQuantite->setValue(1);
}
void MainWindow::on_pushButtonMaListeProduit_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonProduitListe_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonOutilsRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonNouveauRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonCreerProduitFermer_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonCreerCategorieFermer_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonlesRayonsFermer_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonMaListeOutils_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_pushButtonNouveauCreerProduit_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}
void MainWindow::on_pushButtonNouveauCreerCategorie_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_pushButtonCreerProduitRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_pushButtonProduitPlus_clicked()
{
    ui->lineEditProduitRecherche->clear();
    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_pushButtonCreerCategorieRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}
void MainWindow::on_pushButtonOutilsOrdreRayon_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void MainWindow::on_pushButtonlesRayonsRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}
void MainWindow::on_pushButtonRayonsEnregistrer_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //requete enregistre le nouvel ordre des rayons
    for(int i=0;i<vecteurRayon.size();i++)
    {
        QSqlQuery req("update rayon set rayonOrdre="+QString::number(i+1)+" where rayonLibelle='"+ui->listWidgetRayonsModification->item(i)->text()+"'");
    }
    this->chargerListe();
    this->chargerProduits();
    this->chargerRayon();
}
void MainWindow::on_pushButtonRayonsAnnuler_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
    //recharge la liste des rayons
    this->chargerRayon();
    //passe le bouton enregistrer a faux
    ui->pushButtonRayonsEnregistrer->setEnabled(false);
}
void MainWindow::on_pushButtonMaListeRayon_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
void MainWindow::on_pushButtonRayonsProduits_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_pushButtonRayonsListe_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonProduitRayon_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
void MainWindow::on_pushButtonRayonsOutils_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}
void MainWindow::on_pushButtonOutilsRayonNouveauCreerCategorie_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
}
void MainWindow::on_pushButtonOutilsRayonOrdreRayon_clicked()
{
    ui->stackedWidget->setCurrentIndex(7);
}
void MainWindow::on_pushButtonOutilsRayonRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
void MainWindow::on_pushButtonProduitAjouterAMaListe_clicked()
{
    //requete ajouter, récupére le produit a la ligne sélectionné et recupére le nombre de produit
    this->base.exec("update produit set etat=1, quantite="+QString::number(ui->spinBoxProduitsQuantite->value())+" where produitNum="+QString::number(this->vecteurIdProduit.value(ui->listWidgetProduit->currentRow())));
    //recharge la liste et remet a 1 le nombre de produit
    this->chargerProduits();
    this->chargerListe();
}
bool MainWindow::lineEditCreerCategorieOk()
{
    return(ui->lineEditCreerCategorieNomCategorie->text().length());
}
void MainWindow::on_lineEditCreerCategorieNomCategorie_textChanged()
{
    ui->pushButtonCreerCategorieCreer->setEnabled(lineEditCreerCategorieOk());
}
bool MainWindow::lineEditEtComboBoxProduitOk()
{
/// A REVOIR POUR AJOUTER LA COMBOBOX SELECTIONNE
    return(ui->lineEditCreerProduitNom->text().length());
    //ui->comboBoxCreerProduitChoisirCategorie->actions();
}
void MainWindow::on_lineEditCreerProduitNom_textChanged()
{
    ui->pushButtonCreerProduitCreer->setEnabled(lineEditEtComboBoxProduitOk());
}
void MainWindow::on_pushButtonCreerProduitCreer_clicked()
{
    //requete ajouter, récupére le nom du produit dans la line édite et récupére le rayon avec la combobox
    QSqlQuery req;
    req.exec("select max(produitNum)+1 from produit");
    req.first();
    int numero=req.value(0).toInt() +0;
    this->base.exec("insert into produit values ("+QString::number(numero)+",'"+ui->lineEditCreerProduitNom->text().trimmed()+"',"+QString::number(ui->comboBoxCreerProduitChoisirCategorie->currentIndex())+",0,0)");
    //recharge la liste des produits
    this->chargerProduits();
    ui->lineEditCreerProduitNom->clear();
}
void MainWindow::on_pushButtonCreerCategorieCreer_clicked()
{
    //requete pour récupérer le placement de la nouvelle catégorie (max +1)
    QSqlQuery req;
    req.exec("select max(rayonOrdre)+1, max(rayonNum)+1 from rayon");
    req.first();
    int rayonNum=req.value(1).toInt()+0;
    int rayonOrdre=req.value(0).toInt()+0;
    //requete ajouter, récupére le nom de la nouvelle catégorie (rayon)
    this->base.exec("insert into rayon values ("+QString::number(rayonNum)+",'"+ui->lineEditCreerCategorieNomCategorie->text().trimmed()+"','"+QString::number(rayonOrdre)+"')");
    this->chargerRayon();
    ui->lineEditCreerCategorieNomCategorie->clear();
    ui->stackedWidget->setCurrentIndex(6);
}
void MainWindow::on_listWidgetRayonsModification_currentRowChanged()
{
    ui->pushButtonRayonsEnregistrer->setEnabled(true);
}
void MainWindow::on_pushButtonOutilsViderListe_clicked()
{
    //requete qui passe a 0 l'etat de tous les produits
    this->base.exec("update produit set etat=0 ");
    //recharge la liste et le panier
    this->chargerListe();
    this->chargerPanier();
    //place sur la page de la liste
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_pushButtonOutilsRayonModifierRayon_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
}
void MainWindow::on_pushButtonModificationRayonFermer_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);
}
void MainWindow::on_pushButtonModificationRayonRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
}
void MainWindow::on_pushButtonEditerRayonModifier_clicked()
{
    //requete de modification sur le rayon
    this->base.exec("update rayon set rayonLibelle='"+ui->lineEditEditerRayon->text().trimmed()+"' where rayonNum="+QString::number(this->vecteurRayon.value(ui->listWidgetEditerRayon->currentRow())));
    this->chargerComboBoxCreerProduitChoisirCategorie();
    this->chargerListe();
    this->chargerPanier();
    this->chargerProduits();
    this->chargerRayon();
}
void MainWindow::on_pushButtonEditerRayonSupprimer_clicked()
{
    //requete de suppression du rayon
    this->base.exec("delete from rayon where rayonNum="+QString::number(this->vecteurRayon.value(ui->listWidgetEditerRayon->currentRow())));
    this->chargerRayon();
}
void MainWindow::on_listWidgetEditerRayon_clicked()
{
    QSqlQuery req=this->base.exec("select rayonLibelle from rayon where rayonNum="+QString::number(this->vecteurRayon.value(ui->listWidgetEditerRayon->currentRow())));
    if (req.first())
    {
        //affichage dans l'interface
        ui->lineEditEditerRayon->setText(req.value(0).toString());
    }
    ui->pushButtonEditerRayonSupprimer->setEnabled(true);
}
bool MainWindow::editerRayonModifierOk()
{
    return ui->lineEditEditerRayon->text().length();
}
void MainWindow::on_lineEditEditerRayon_textChanged()
{
    ui->pushButtonEditerRayonModifier->setEnabled(editerRayonModifierOk());
}
void MainWindow::on_lineEditProduitRecherche_textChanged()
{
    ui->listWidgetProduit->clear();
    vecteurIdProduit.clear();
    currentRayonAffiche="null";
    QSqlQuery req("select produitLibelle, produitNum, rayonNum from produit where produitLibelle like'%"+ui->lineEditProduitRecherche->text().trimmed()+"%'");
    while(req.next())
    {
        QLinearGradient linGrad;
        QSqlQuery reqRayon("select rayonLibelle from rayon where rayonNum="+req.value(2).toString());
        reqRayon.first();
        if(reqRayon.value(0).toString()!=currentRayonAffiche)
        {
            currentRayonAffiche=reqRayon.value(0).toString();
            QListWidgetItem *itemRayon=new QListWidgetItem("- "+reqRayon.value(0).toString());
            itemRayon->setFlags(Qt::NoItemFlags);
            ui->listWidgetProduit->addItem(itemRayon);
            linGrad.setColorAt( 0, QColor(250,250,150));//couleur
            QBrush brush(linGrad);//definit une QBrush
            itemRayon->setBackground(brush);
            itemRayon->setForeground(QBrush(Qt::black));
        }
        QListWidgetItem *myItem=new QListWidgetItem(req.value(0).toString());
        ui->listWidgetProduit->addItem(myItem);
        vecteurIdProduit.push_back(req.value(1).toInt());
    }
}
void MainWindow::on_pushButtonModificationProduitRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_pushButtonGestionProduitEditProduit_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);
}
void MainWindow::on_pushButtonModificationProduitFermer_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_listWidgetModificationProduit_clicked()
{
    QSqlQuery req=this->base.exec("select produitLibelle from produit where produitNum="+QString::number(this->vecteurIdProduit.value(ui->listWidgetModificationProduit->currentRow())));
    if (req.first())
    {
        //affichage dans l'interface
        ui->lineEditModificationProduit->setText(req.value(0).toString());
    }
    ui->pushButtonEditerProduitSupprimer->setEnabled(true);
}
void MainWindow::on_pushButtonEditerProduitSupprimer_clicked()
{
    //requete de suppression du produit
    this->base.exec("delete from produit where produitNum="+QString::number(this->vecteurIdProduit.value(ui->listWidgetModificationProduit->currentRow())));
    this->chargerProduits();
    this->chargerListe();
    this->chargerPanier();
    ui->lineEditModificationProduit->clear();
}
void MainWindow::on_pushButtonEditerProduitModifier_clicked()
{
    //requete de modification
    this->base.exec("update produit set produitLibelle='"+ui->lineEditModificationProduit->text().trimmed()+"' where produitNum="+QString::number(this->vecteurIdProduit.value(ui->listWidgetModificationProduit->currentRow())));
    this->chargerProduits();
    this->chargerListe();
    this->chargerPanier();
    ui->lineEditModificationProduit->clear();
}
bool MainWindow::lineEditModificationProduitOk()
{
    return ui->lineEditModificationProduit->text().trimmed().length();
}
void MainWindow::on_lineEditModificationProduit_textChanged()
{
    ui->pushButtonEditerProduitModifier->setEnabled(lineEditModificationProduitOk());
}
void MainWindow::on_pushButtonQuantitePlus_clicked()
{
    ui->spinBoxProduitsQuantite->setValue(ui->spinBoxProduitsQuantite->value()+1);
}
void MainWindow::on_pushButtonQuantiteMoins_clicked()
{
    ui->spinBoxProduitsQuantite->setValue(ui->spinBoxProduitsQuantite->value()-1);
}
void MainWindow::on_listWidgetMaListeAAcheter_clicked()
{
    //requete de modification d'etat
    this->base.exec("update produit set etat=2 where produitNum="+QString::number(this->vecteurListeAAcheter.value(ui->listWidgetMaListeAAcheter->currentRow())));
    this->chargerListe();
    this->chargerPanier();
}
void MainWindow::on_listWidgetMaListePanier_clicked()
{
    this->base.exec("update produit set etat=1 where produitNum="+QString::number(this->vecteurPanier.value(ui->listWidgetMaListePanier->currentRow())));
    this->chargerListe();
    this->chargerPanier();
}
