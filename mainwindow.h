#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QVector>
#include <QModelIndex>
#include <QListWidgetItem>
#include <QString>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSqlDatabase base;
    void ouvreBase();
    void verifBase();
    void chargerComboBoxCreerProduitChoisirCategorie();
    void chargerListe();
    void chargerPanier();
    void chargerProduits();
    void chargerRayon();
    //void chargerRayon();
    void fermeBase();
    void actualiserOrdre();
    QVector <int> vecteurPanier, vecteurRayon, vecteurIdProduit, vecteurListeAAcheter;
    bool lineEditCreerCategorieOk();
    bool lineEditEtComboBoxProduitOk();
    bool editerRayonModifierOk();
    bool lineEditModificationProduitOk();
    //QVector <QString> vecteurListeAAcheter;
    QString currentRayonAffiche;






private slots:
    void on_pushButtonMaListeProduit_clicked();
    void on_pushButtonProduitListe_clicked();
    void on_pushButtonOutilsRetour_clicked();
    void on_pushButtonNouveauRetour_clicked();
    void on_pushButtonCreerProduitFermer_clicked();
    void on_pushButtonCreerCategorieFermer_clicked();
    void on_pushButtonlesRayonsFermer_clicked();
    void on_pushButtonMaListeOutils_clicked();
    void on_pushButtonNouveauCreerProduit_clicked();
    void on_pushButtonNouveauCreerCategorie_clicked();
    void on_pushButtonCreerProduitRetour_clicked();
    void on_pushButtonProduitPlus_clicked();
    void on_pushButtonCreerCategorieRetour_clicked();
    void on_pushButtonOutilsOrdreRayon_clicked();
    void on_pushButtonlesRayonsRetour_clicked();
    void on_pushButtonRayonsEnregistrer_clicked();
    void on_pushButtonRayonsAnnuler_clicked();
    void on_pushButtonMaListeRayon_clicked();
    void on_pushButtonRayonsProduits_clicked();
    void on_pushButtonRayonsListe_clicked();
    void on_pushButtonProduitRayon_clicked();
    void on_pushButtonRayonsOutils_clicked();
    void on_pushButtonOutilsRayonNouveauCreerCategorie_clicked();
    void on_pushButtonOutilsRayonOrdreRayon_clicked();
    void on_pushButtonOutilsRayonRetour_clicked();
    void on_pushButtonProduitAjouterAMaListe_clicked();
    void on_lineEditCreerCategorieNomCategorie_textChanged();
    void on_lineEditCreerProduitNom_textChanged();
    void on_pushButtonCreerProduitCreer_clicked();
    void on_pushButtonCreerCategorieCreer_clicked();
    void on_listWidgetRayonsModification_currentRowChanged();
    void on_pushButtonOutilsViderListe_clicked();
    void on_pushButtonOutilsRayonModifierRayon_clicked();

    void on_pushButtonModificationRayonFermer_clicked();

    void on_pushButtonModificationRayonRetour_clicked();

    void on_listWidgetEditerRayon_clicked();

    void on_pushButtonEditerRayonModifier_clicked();

    void on_pushButtonEditerRayonSupprimer_clicked();
    void on_lineEditEditerRayon_textChanged();

    void on_lineEditProduitRecherche_textChanged();

    void on_pushButtonModificationProduitRetour_clicked();

    void on_pushButtonGestionProduitEditProduit_clicked();

    void on_pushButtonModificationProduitFermer_clicked();

    void on_listWidgetModificationProduit_clicked();

    void on_pushButtonEditerProduitSupprimer_clicked();

    void on_pushButtonEditerProduitModifier_clicked();

    void on_lineEditModificationProduit_textChanged();

    void on_pushButtonQuantitePlus_clicked();

    void on_pushButtonQuantiteMoins_clicked();

    void on_listWidgetMaListeAAcheter_clicked();

    void on_listWidgetMaListePanier_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
